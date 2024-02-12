#include "svg.h"
#include <utility>
#include <string>

namespace svg {

using namespace std::literals;
    
std::ostream& operator<<(std::ostream& stream, const StrokeLineCap& stroke_line_cap) {
    switch (stroke_line_cap) {
        case StrokeLineCap::BUTT:
            stream << "butt";
            break;
        case StrokeLineCap::ROUND:
            stream << "round";
            break;
        case StrokeLineCap::SQUARE:
            stream << "square";
            break;                 
    }    
    return stream;
}
    
std::ostream& operator<<(std::ostream& stream, const StrokeLineJoin& stroke_line_join) {
    switch (stroke_line_join) {
        case StrokeLineJoin::ARCS:
            stream << "arcs";
            break;
        case StrokeLineJoin::BEVEL:
            stream << "bevel";
            break;
        case StrokeLineJoin::MITER:
            stream << "miter";
            break;
        case StrokeLineJoin::MITER_CLIP:
            stream << "miter-clip";
            break;
        case StrokeLineJoin::ROUND:
            stream << "round";
            break; 
    }    
    return stream;
}
    
std::ostream& operator<<(std::ostream& out, const Color& color) {
    if (std::holds_alternative<std::monostate>(color)) {
        out << "none";
        return out;
    }
    if (std::holds_alternative<std::string>(color)) {
        if (std::get<std::string>(color) == ""s) {
            out << "none";
        } else {
            out << std::get<std::string>(color);
        }           
        return out;
    }
    
    if (std::holds_alternative<Rgb>(color)) {
        out << "rgb(" << (int)std::get<Rgb>(color).red
        << "," << (int)std::get<Rgb>(color).green
        << "," << (int)std::get<Rgb>(color).blue << ")";
        return out;
    }
    if (std::holds_alternative<Rgba>(color)) {
        out << "rgba(" << (int)std::get<Rgba>(color).red << "," << (int)std::get<Rgba>(color).green << "," << (int)std::get<Rgba>(color).blue << ","
        << (double)std::get<Rgba>(color).opacity << ")";
        return out;
    }
    return out;
}  


void Object::Render(const RenderContext& context) const {
    context.RenderIndent();
    // Делегируем вывод тега своим подклассам
    RenderObject(context);

    context.out << std::endl;
}    

// ---------- Circle ------------------

Circle& Circle::SetCenter(Point center)  {
    center_ = center;
    return *this;
}

Circle& Circle::SetRadius(double radius)  {
    radius_ = radius;
    return *this;
}

void Circle::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
    out << "r=\""sv << radius_ << "\" "sv;
    RenderAttrs(context.out);    
    out << "/>"sv;
}
    
// ---------- Polyline ------------------
    
Polyline& Polyline::AddPoint(Point point) {
    points_.push_back(point);
    return *this;
}
    
void Polyline::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<polyline points=\""s;
    bool is_first = true;
    for (Point point : points_) {
        if (is_first) {
            out << point.x << "," << point.y;
            is_first = false;
        } else {
            out << " " << point.x << "," << point.y;
        }
    }
    out << "\"";
    RenderAttrs(context.out);
    out << " />"s;
}
    
// ---------- Text ------------------
    
Text& Text::SetPosition(Point pos) {
    position_ = pos;
    return *this;
}

    // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
Text& Text::SetOffset(Point offset) {
    offset_ = offset;
    return *this;
}
    // Задаёт размеры шрифта (атрибут font-size)
Text& Text::SetFontSize(uint32_t size) {
    size_ = size;
    return *this;
}
    
    // Задаёт название шрифта (атрибут font-family)
Text& Text::SetFontFamily(std::string font_family) {
    font_family_ = font_family;
    return *this;
}

    // Задаёт толщину шрифта (атрибут font-weight)
Text& Text::SetFontWeight(std::string font_weight) {
    font_weight_ = font_weight;
    return *this;
}

    // Задаёт текстовое содержимое объекта (отображается внутри тега text)
Text& Text::SetData(std::string data) {
    data_ = data;
    return *this;
}

void Text::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<text x=\"" << position_.x << "\" y=\"" << position_.y << "\" dx=\"" << offset_.x << "\" dy=\"" << offset_.y
        << "\" font-size=\"" << size_ << "\"";
    if(!font_family_.empty()) {
        out << " font-family=\"" << font_family_ << "\"";
    }
    
    if(!font_weight_.empty()) {
        out << " font-weight=\"" << font_weight_ << "\"";
    }

    std::string data_correct_format;
    for (char symbol : data_) {        
        if (symbol == '\"') {
            data_correct_format.append("&quot;");
            continue;
        }
        if (symbol == '<') {
            data_correct_format.append("&lt;");
            continue;
        }
        if (symbol == '>') {
            data_correct_format.append("&gt;");
            continue;
        }
        if (symbol == '\'') {
            data_correct_format.append("&apos;");
            continue;
        }
        if (symbol == '&') {
            data_correct_format.append("&amp;");
            continue;
        }
        data_correct_format.append(1, symbol);        
    }
    RenderAttrs(context.out);
    out << ">" << data_correct_format << "</text>";
}
// ----------ContainerObject-------------
void ObjectContainer::Render(std::ostream& out) const {
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << std::endl;
    RenderContext context(out);
    for (auto& object : objects_) {
        out << "  ";
        object->Render(context);
    }
    out << "</svg>"sv;
}    
    
// ---------- Document ------------------
void Document::AddPtr(std::unique_ptr<Object>&& obj) {
    objects_.emplace_back(move(obj));
}
}  // namespace svg