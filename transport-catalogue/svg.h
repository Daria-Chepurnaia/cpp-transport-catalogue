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
 * Вспомогательная структура, хранящая контекст для вывода SVG-документа с отступами.
 * Хранит ссылку на поток вывода, текущее значение и шаг отступа при выводе элемента
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
 * Абстрактный базовый класс Object служит для унифицированного хранения
 * конкретных тегов SVG-документа
 * Реализует паттерн "Шаблонный метод" для вывода содержимого тега
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

    // Метод RenderAttrs выводит в поток общие для всех путей атрибуты fill и stroke
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
        // static_cast безопасно преобразует *this к Owner&,
        // если класс Owner — наследник PathProps
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
    
    // Выводит в ostream svg-представление документа
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
    // Задаёт координаты опорной точки (атрибуты x и y)
    Text& SetPosition(Point pos);

    // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
    Text& SetOffset(Point offset);

    // Задаёт размеры шрифта (атрибут font-size)
    Text& SetFontSize(uint32_t size);

    // Задаёт название шрифта (атрибут font-family)
    Text& SetFontFamily(std::string font_family);

    // Задаёт толщину шрифта (атрибут font-weight)
    Text& SetFontWeight(std::string font_weight);

    // Задаёт текстовое содержимое объекта (отображается внутри тега text)
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
    // Добавляет в svg-документ объект-наследник svg::Object
    void AddPtr(std::unique_ptr<Object>&& obj) override; 
};
 
}  // namespace svg