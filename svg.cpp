#include "svg.h"

namespace svg {

    using namespace std::literals;

    //////////////////////////////////////////////////////////////////////////////////////
    //                                                                                  //
    //                         STRUCTURES & ENUMS & OTHER                               //
    //                                                                                  //
    //////////////////////////////////////////////////////////////////////////////////////

    std::ostream& operator<<(std::ostream& os, const StrokeLineCap& lc) {
        switch (lc)
        {
        case StrokeLineCap::BUTT:
            os << "butt";
            break;

        case StrokeLineCap::ROUND:
            os << "round";
            break;

        case StrokeLineCap::SQUARE:
            os << "square";
            break;

        }
        return os;
    }

    std::ostream& operator<<(std::ostream& os, const StrokeLineJoin& lc) {
        switch (lc)
        {
        case StrokeLineJoin::ARCS:
            os << "arcs";
            break;

        case StrokeLineJoin::BEVEL:
            os << "bevel";
            break;

        case StrokeLineJoin::MITER:
            os << "miter";
            break;
        case StrokeLineJoin::MITER_CLIP:
            os << "miter-clip";
            break;
        case StrokeLineJoin::ROUND:
            os << "round";
            break;
        }
        return os;
    }

    
    std::ostream& operator<<(std::ostream& os, const Rgb& rgb) {
        os << "rgb(" << static_cast<int>(rgb.red) << ',' << static_cast<int>(rgb.green) << ',' << static_cast<int>(rgb.blue) << ')';
        return os;
    }


    std::ostream& operator<<(std::ostream& os, const Rgba& rgba) {
        os << "rgba(" << static_cast<int>(rgba.red) << ',' << static_cast<int>(rgba.green)<<',' << static_cast<int>(rgba.blue) << ',' << rgba.opacity << ')';
        return os;
    }
 
    std::ostream& operator<<(std::ostream& os, const Color& color) {
        std::visit(Visitor{ os }, color);
        return os;
    }

    Point::Point(double x, double y)
        : x(x)
        , y(y) {
    }

    RenderContext::RenderContext(std::ostream& out)
        : out(out) {
    }

    RenderContext::RenderContext(std::ostream& out, int indent_step, int indent = 0)
        : out(out)
        , indent_step(indent_step)
        , indent(indent) {
    }

    RenderContext  RenderContext::Indented() const {
        return { out, indent_step, indent + indent_step };
    }

    void  RenderContext::RenderIndent() const {
        for (int i = 0; i < indent; ++i) {
            out.put(' ');
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////
    //                                                                                  //
    //                                     OBJECT                                       //
    //                                                                                  //
    //////////////////////////////////////////////////////////////////////////////////////

    void Object::Render(const RenderContext& context) const {
        context.RenderIndent();
        // Делегируем вывод тега своим подклассам
        RenderObject(context);
        context.out << std::endl;
    }

    //////////////////////////////////////////////////////////////////////////////////////
    //                                                                                  //
    //                                     CIRCLE                                       //
    //                                                                                  //
    //////////////////////////////////////////////////////////////////////////////////////

    Circle& Circle::SetCenter(Point center) {
        center_ = center;
        return *this;
    }

    Circle& Circle::SetRadius(double radius) {
        radius_ = radius;
        return *this;
    }

    void Circle::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\""sv;
        out << " r=\""sv << radius_ << "\""sv;
        RenderAttrs(out);
        out << "/>"sv;
    }

    //////////////////////////////////////////////////////////////////////////////////////
    //                                                                                  //
    //                                       TEXT                                       //
    //                                                                                  //
    //////////////////////////////////////////////////////////////////////////////////////

    // Задаёт координаты опорной точки (атрибуты x и y)
    Text& Text::SetPosition(Point pos) {
        start_ = std::move(pos);
        return *this;
    }

    // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
    Text& Text::SetOffset(Point offset) {
        offset_ = std::move(offset);
        return *this;
    }

    // Задаёт размеры шрифта (атрибут font-size)
    Text& Text::SetFontSize(uint32_t size) {
        font_size_ = size;
        return *this;
    }

    // Задаёт название шрифта (атрибут font-family)
    Text& Text::SetFontFamily(std::string font_family) {
        font_family_ = std::move(font_family);
        return *this;
    }

    // Задаёт толщину шрифта (атрибут font-weight)
    Text& Text::SetFontWeight(std::string font_weight) {
        font_weight_ = std::move(font_weight);
        return *this;
    }

    // Задаёт текстовое содержимое объекта (отображается внутри тега text)
    Text& Text::SetData(std::string data) {
        text_ = std::move(data);
        return *this;
    }

   
    void Text::RenderObject(const RenderContext& context)  const {
        auto& out = context.out;
        out << "<text";
        RenderAttrs(out);
        out<< " x=\"" << start_.x << "\" y=\"" << start_.y << "\"";
        out << " dx=\"" << offset_.x << "\" dy=\"" << offset_.y << "\"";
        out << " font-size=\"" << font_size_ << "\"";
        if (!font_family_.empty()) { out << " font-family=\"" << font_family_ << "\""; }
        if (!font_weight_.empty()) out << " font-weight=\"" << font_weight_ << "\"";
        out << ">";
        for (const char i : text_) {
            switch (i) {
            case '"':
                out << "&quot;";
                break;
            case '\'':
                out << "&apos;";
                break;
            case '<':
                out << "&lt;";
                break;
            case '>':
                out << "&gt;";
                break;
            case '&':
                out << "&amp;";
                break;
            default:
                out << i;
                break;
            }
        }
        out << "</text>";
    }

    //////////////////////////////////////////////////////////////////////////////////////
    //                                                                                  //
    //                                       POLYLINE                                   //
    //                                                                                  //
    //////////////////////////////////////////////////////////////////////////////////////

    Polyline& Polyline::AddPoint(Point point) {
        points_.emplace_back(std::move(point));
        return *this;
    }


    void  Polyline::RenderObject(const RenderContext& context)  const {
        auto& out = context.out;
        out << "<polyline points=\"";
        for (size_t i = 0; i < points_.size(); ++i) {
            out << points_[i].x << ',' << points_[i].y;
            if (i == points_.size() - 1) { break; }
            out << ' ';
        }
        out << "\"";
        RenderAttrs(out);
        out << "/>";
    }

    //////////////////////////////////////////////////////////////////////////////////////
    //                                                                                  //
    //                                       DOCUMENT                                   //
    //                                                                                  //
    //////////////////////////////////////////////////////////////////////////////////////

// Добавляет в svg-документ объект-наследник svg::Object
    void Document::AddPtr(std::unique_ptr<Object>&& obj) {
        objects_.emplace_back(std::move(obj));
    }

    // Выводит в ostream svg-представление документа
    void Document::Render(std::ostream& out) const {
        out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << std::endl;
        out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">" << std::endl;
        for (auto&& i : objects_) {
            out << "  ";
            i->Render(out);
        }
        out << "</svg>" << std::endl;
    }

}  // namespace svg