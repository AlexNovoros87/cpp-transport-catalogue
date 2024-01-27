#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <optional>
#include <variant>

    namespace svg {


        //////////////////////////////////////////////////////////////////////////////////////
        //                                                                                  //
        //                         STRUCTURES & ENUMS & OTHER                               //
        //                                                                                  //
        //////////////////////////////////////////////////////////////////////////////////////

        struct Rgb {
       
            Rgb() = default;
            Rgb(int r, int g, int b):red(static_cast<uint8_t>(r)),green(static_cast<uint8_t>(g)), blue(static_cast<uint8_t>(b)) {};
            uint8_t red =0 ;
            uint8_t  green = 0;
            uint8_t blue = 0;
        };
    
        struct Rgba {
         
        
            Rgba() = default;
            Rgba(int r, int g, int b, double o) :red(static_cast<uint8_t>(r)), green(static_cast<uint8_t>(g)), blue(static_cast<uint8_t>(b)), opacity(o) {};
            uint8_t  red = 0;
            uint8_t  green = 0;
            uint8_t  blue = 0 ;
            double opacity = 1.0;
        };

  
        //   using Color = std::string;
        using Color = std::variant<std::monostate,std::string, Rgb, Rgba>;
        const Color NoneColor{"none"};

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


        std::ostream& operator<<(std::ostream& os, const StrokeLineCap& lc);
        std::ostream& operator<<(std::ostream& os, const StrokeLineJoin& lc);
        std::ostream& operator<<(std::ostream& os, const Rgb& rgb);
        std::ostream& operator<<(std::ostream& os, const Rgba& rgba);

   
    

    
        struct Visitor {
            std::ostream& os;
            void operator()([[maybe_unused]] std::monostate ms) {
                os << "none";
            }
            void operator()(const std::string& str) {
                os << str;
            }
            void operator()(const Rgb& rgb) {
                os << rgb;

            }
            void operator()(const Rgba& rgba) {
                os << rgba;

            }
        };
    
        std::ostream& operator<<(std::ostream& os, const Color& color);
    
    
        struct Point {
            Point() = default;
            Point(double x, double y);
            double x = 0;
            double y = 0;
        };


        /*
            * Вспомогательная структура, хранящая контекст для вывода SVG-документа с отступами.
            * Хранит ссылку на поток вывода, текущее значение и шаг отступа при выводе элемента
            */


        struct RenderContext {
            RenderContext(std::ostream& out);
            RenderContext(std::ostream& out, int indent_step, int indent);
            RenderContext Indented() const;
            void RenderIndent() const;

            std::ostream& out;
            int indent_step = 0;
            int indent = 0;
        };



        /////////////////////////////////////////////////////////////////////////////////////////
        //                                                                                     // 
        //                                   OBJECT                                            //
        //*                                                                                    //
        // * Абстрактный базовый класс Object служит для унифицированного хранения             //
        // * конкретных тегов SVG-документа                                                    //
        // * Реализует паттерн "Шаблонный метод" для вывода содержимого тега                   //
        // */                                                                                  //
        /////////////////////////////////////////////////////////////////////////////////////////

        class Object {
        public:
            void Render(const RenderContext& context) const;
            virtual ~Object() = default;

        private:
            virtual void RenderObject(const RenderContext& context) const = 0;
        };



        //////////////////////////////////////////////////////////////////////////////////////
        //                                                                                   //
        //                                       PATHPROPS                                   //
        //                                                                                   //
        /////////////////////////////////////////////////////////////////////////////////////// 


        template <typename Owner>
        class PathProps {
        public:
            Owner& SetFillColor(Color color);
            Owner& SetStrokeColor(Color color);
            Owner& SetStrokeWidth(double width);
            Owner& SetStrokeLineCap(StrokeLineCap line_cap);
            Owner& SetStrokeLineJoin(StrokeLineJoin line_join);


        protected:
            ~PathProps() = default;
            // Метод RenderAttrs выводит в поток общие для всех путей атрибуты fill и stroke
            void RenderAttrs(std::ostream& out) const;

        private:
            Owner& AsOwner();
            std::optional<Color> fill_color_;
            std::optional<Color> stroke_color_;
            std::optional<double> stroke_width_;
            std::optional<StrokeLineCap> stroke_line_cap_;
            std::optional<StrokeLineJoin> stroke_line_join_;
        };


        template <typename Owner>
        Owner& PathProps<Owner>::SetFillColor(Color color) {
            fill_color_ = std::move(color);
            return AsOwner();
        }
    
        template <typename Owner>
        Owner& PathProps<Owner>::SetStrokeColor(Color color) {
            stroke_color_ = std::move(color);
            return AsOwner();
        }
        template <typename Owner>
        Owner& PathProps<Owner>::SetStrokeWidth(double width) {
            stroke_width_ = width;
            return AsOwner();
        }
        template <typename Owner>
        Owner& PathProps<Owner>::SetStrokeLineCap(StrokeLineCap line_cap) {
            stroke_line_cap_ = std::move(line_cap);
            return AsOwner();
        }
        template <typename Owner>
        Owner& PathProps<Owner>::SetStrokeLineJoin(StrokeLineJoin line_join) {
            stroke_line_join_ = std::move(line_join);
            return AsOwner();
        }
   
        template <typename Owner>
        void PathProps<Owner>::RenderAttrs(std::ostream& out) const {
            using namespace std::literals;

            if (fill_color_) {
                out << " fill=\"" << *fill_color_ << "\""sv;
            }
            if (stroke_color_) {
                out << " stroke=\"" << *stroke_color_ << "\""sv;
            }
            if (stroke_width_) {
                out << " stroke-width=\"" << *stroke_width_ << "\""sv;
            }
            if (stroke_line_cap_) {
                out << " stroke-linecap=\"" << *stroke_line_cap_ << "\""sv;
            }
            if (stroke_line_join_) {
                out << " stroke-linejoin=\"" << *stroke_line_join_ << "\""sv;
            }
        }

        template <typename Owner>
        Owner& PathProps<Owner>::AsOwner() {
                // static_cast безопасно преобразует *this к Owner&,
                // если класс Owner — наследник PathProps
            return static_cast<Owner&>(*this);
        }

        //////////////////////////////////////////////////////////////////////////////////////
            //                                                                                  //
            //                                    CIRCLE                                        //          
            //          Класс Circle моделирует элемент <circle> для отображения круга          //  
            //      https://developer.mozilla.org/en-US/docs/Web/SVG/Element/circle             //
            //                                                                                  //
            ////////////////////////////////////////////////////////////////////////////////////// 



        class Circle final : public Object, public PathProps<Circle> {
        public:
            Circle& SetCenter(Point center);
            Circle& SetRadius(double radius);

        private:
            void RenderObject(const RenderContext& context) const override;

            Point center_;
            double radius_ = 1.0;
        };

        //////////////////////////////////////////////////////////////////////////////////////
        //                                                                                  //
        //                                     POLYLINE                                     //          
        // Класс Polyline моделирует элемент <polyline> для отображения ломаных линий       //  
        // *https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polyline               //
        //                                                                                  //
        ////////////////////////////////////////////////////////////////////////////////////// 


        class Polyline : public Object, public PathProps<Polyline> {
        public:
            // Добавляет очередную вершину к ломаной линии
            Polyline& AddPoint(Point point);

        private:
            std::vector<Point> points_;
            void RenderObject(const RenderContext& context)  const override;
        };


        //////////////////////////////////////////////////////////////////////////////////////
        //                                                                                  //
        //                                     TEXT                                         //          
        //                  Класс Text моделирует элемент <text> для отображения текста     //  
        //          https://developer.mozilla.org/en-US/docs/Web/SVG/Element/text           //
        //                                                                                  //
        ////////////////////////////////////////////////////////////////////////////////////// 

        class Text :public Object, public PathProps<Text> {
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

            // Прочие данные и методы, необходимые для реализации элемента <text>

        private:
            void RenderObject(const RenderContext& context)  const override;

            Point start_ = { 0,0 };
            Point offset_ = { 0,0 };
            uint32_t font_size_ = 1;
            std::string font_family_;
            std::string font_weight_;
            std::string text_ = "";
        };

        //////////////////////////////////////////////////////////////////////////////////////
        //                                                                                  //
        //                                  INTERFACES                                      //
        //                                                                                  //
        //////////////////////////////////////////////////////////////////////////////////////  

        class ObjectContainer {
        public:
            template<typename Obj>
            void Add(Obj obj);
            virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;
            virtual ~ObjectContainer() = default;
        protected:
            std::vector<std::unique_ptr<Object>> objects_;
        };

        template<typename Obj>
        void ObjectContainer::Add(Obj obj) {
            objects_.emplace_back(std::make_unique<Obj>(std::move(obj)));
        }

        class Drawable
        {
        public:
            virtual ~Drawable() = default;
            virtual void Draw(ObjectContainer& cont) const = 0;
        };

        //////////////////////////////////////////////////////////////////////////////////////
        //                                                                                  //
        //                                  DOCUMENT                                        //
        //                                                                                  //
        //////////////////////////////////////////////////////////////////////////////////////


        class Document : public ObjectContainer {
        public:
            // Добавляет в svg-документ объект-наследник svg::Object
            void AddPtr(std::unique_ptr<Object>&& obj) override;

            // Выводит в ostream svg-представление документа
            void Render(std::ostream& out) const;
        };

    }  // namespace svg