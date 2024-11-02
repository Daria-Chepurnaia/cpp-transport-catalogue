#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace svg {
    
namespace detail {
template <typename AttrType>
inline void RenderAttr(std::ostream& out, std::string_view name, const AttrType& value) {
    using namespace std::literals;
    out << name << "=\""sv;
    out << value;
    out.put('"');
}

template <typename AttrType>
inline void RenderOptionalAttr(std::ostream& out, std::string_view name, const std::optional<AttrType>& value) {
    if (value) {
        RenderAttr(out, name, *value);
    }
}    
} // namespace detail

struct Rgb {
    uint8_t red{};
    uint8_t green{};
    uint8_t blue{};
};

struct Rgba  {
    uint8_t red{};
    uint8_t green{};
    uint8_t blue{};
    double opacity = 1.0;
};     

using Color = std::variant<std::monostate, std::string, Rgb, Rgba>;
inline const Color NoneColor{"none"};

enum class StrokeLineCap {
    BUTT,
    ROUND,
    SQUARE,
};

enum class StrokeLineJoin {
    ARCS,
    BEVEL,
    MITER,
    MITER_CLIP,
    ROUND,
};   

std::ostream& operator<<(std::ostream& stream, const StrokeLineCap& stroke_line_cap);    
std::ostream& operator<<(std::ostream& stream, const StrokeLineJoin& stroke_line_join);
std::ostream& operator<<(std::ostream& stream, const Color& color); 
    
struct Point {
    Point() = default;
    Point(double x, double y)
        : x(x)
        , y(y) {
    }
    double x = 0;
    double y = 0;
};

/*
 * Helper structure that stores the context for outputting the SVG document with indentation.
 * Stores a reference to the output stream, the current indentation level, and the indentation step
 * when outputting an element.
 */
struct RenderContext {
    RenderContext(std::ostream& out)
        : out(out) {
    }

    RenderContext(std::ostream& out, int indent_step, int indent = 0)
        : out(out)
        , indent_step(indent_step)
        , indent(indent) {
    }

    RenderContext Indented() const {
        return {out, indent_step, indent + indent_step};
    }

    void RenderIndent() const {
        for (int i = 0; i < indent; ++i) {
            out.put(' ');
        }
    }

    std::ostream& out;
    int indent_step = 0;
    int indent = 0;
};

/*
 * Abstract base class Object serves for unified storage
 * of specific SVG document tags.
 * Implements the "Template Method" pattern for outputting tag content.
 */
template <typename Owner>
class PathProps {
public:
    Owner& SetFillColor(Color color) {
        fill_color_ = std::move(color);
        return AsOwner();
    }
    
    Owner& SetStrokeColor(Color color) {
        stroke_color_ = std::move(color);
        return AsOwner();
    }
    
    Owner& SetStrokeWidth(double width) {
        width_ = std::move(width);
        return AsOwner();
    }
    
    Owner& SetStrokeLineCap(StrokeLineCap line_cap) {
        line_cap_ = std::move(line_cap);
        return AsOwner();
    }
    
    Owner& SetStrokeLineJoin(StrokeLineJoin line_join) {
        line_join_ = std::move(line_join);
        return AsOwner();
    }

protected:
    ~PathProps() = default;

    // The RenderAttrs method outputs common attributes fill and stroke for all paths to the stream.
    void RenderAttrs(std::ostream& out) const {
        using detail::RenderOptionalAttr;
        using namespace std::literals;
        RenderOptionalAttr(out, " fill"sv, fill_color_);
        RenderOptionalAttr(out, " stroke"sv, stroke_color_);
        RenderOptionalAttr(out, " stroke-width"sv, width_);
        RenderOptionalAttr(out, " stroke-linecap"sv, line_cap_);
        RenderOptionalAttr(out, " stroke-linejoin"sv, line_join_);
    }

private:    
    Owner& AsOwner() {
        // static_cast safely casts *this to Owner&,
        // if the Owner class is a descendant of PathProps.
        return static_cast<Owner&>(*this);
    }

    std::optional<Color> fill_color_;
    std::optional<Color> stroke_color_;
    std::optional<double> width_;
    std::optional <StrokeLineCap> line_cap_;
    std::optional <StrokeLineJoin> line_join_;
};
    
class Object {
public:    
    void Render(const RenderContext& context) const;
    virtual ~Object() = default;
    
private:
    virtual void RenderObject(const RenderContext& context) const = 0;
};

class Circle final : public Object, public PathProps<Circle> {
public:
    Circle& SetCenter(Point center);
    Circle& SetRadius(double radius);

private:
    void RenderObject(const RenderContext& context) const override;

    Point center_;
    double radius_ = 1.0;
};
    
class ObjectContainer {
public:
    
    template <typename Obj>
    void Add(Obj obj) {
        objects_.emplace_back(std::make_unique<Obj>(std::move(obj)));
    }   
    
    virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;
    
    // Outputs the SVG representation of the document to the ostream.
    void Render(std::ostream& out) const;

    virtual ~ObjectContainer() {}
protected:
    std::vector<std::unique_ptr<Object>> objects_;
};
        
class Drawable {
public:    
    virtual void Draw(ObjectContainer& container) const = 0;
    virtual ~Drawable() {}
}; 
    
class Polyline final : public Object, public PathProps<Polyline> {
public:    
    Polyline& AddPoint(Point point);
private:
    void RenderObject(const RenderContext& context) const override;    
    std::vector<Point> points_;
};

class Text final : public Object, public PathProps<Text> {
public:
    // Sets the coordinates of the anchor point (attributes x and y)
    Text& SetPosition(Point pos);

    // Sets the offset from the anchor point (attributes dx, dy)
    Text& SetOffset(Point offset);

    // Sets the font size (attribute font-size)
    Text& SetFontSize(uint32_t size);

    // Sets the font name (attribute font-family)
    Text& SetFontFamily(std::string font_family);

    // Sets the font weight (attribute font-weight)
    Text& SetFontWeight(std::string font_weight);

    // Sets the text content of the object (displayed inside the text tag)
    Text& SetData(std::string data);

private:
    void RenderObject(const RenderContext& context) const override;
    
    Point position_;
    Point offset_;
    uint32_t size_ = 1;
    std::string font_family_;
    std::string font_weight_;
    std::string data_;
};

class Document : public ObjectContainer {
public:
    // Adds an object derived from svg::Object to the SVG document
    void AddPtr(std::unique_ptr<Object>&& obj) override; 
};
 
}  // namespace svg
