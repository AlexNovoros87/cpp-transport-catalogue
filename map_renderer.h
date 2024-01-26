#pragma once
#include<cmath>
#include<algorithm>
#include<unordered_map>
#include<map>
#include<set>


///////////////////////
#include"geo.h"     ///
#include"domain.h"  ///
#include"svg.h"     ///
///////////////////////

const double EPSILON = 1e-6;
bool IsZero(double value);
/////////////////////////////////////////////////////
//                                                 //
//                SPHERE PROJECTOR                 //
//                                                 //
//                                                 //
//                                                 //
//                                                 //
//                                                 //
//                                                 //
//                                                 //
/////////////////////////////////////////////////////

class SphereProjector {
public:
    // points_begin и points_end задают начало и конец интервала элементов geo::Coordinates
    template <typename PointInputIt>
    SphereProjector(PointInputIt points_begin, PointInputIt points_end,
        double max_width, double max_height, double padding);

    // Проецирует широту и долготу в координаты внутри SVG-изображения
    svg::Point operator()(geo::Coordinates coords) const;

private:
    double padding_;
    double min_lon_ = 0;
    double max_lat_ = 0;
    double zoom_coeff_ = 0;
};



/////////////////////////////////////////////////////
//                                                 //
//                     PREREND                     //
//                                                 //
//                                                 //
//                                                 //
//                                                 //
//                                                 //
//                                                 //
//                                                 //
/////////////////////////////////////////////////////

template<typename Render, typename Base>
class Prerend {
public:

    //конструктор
    Prerend(Render&& render, const Base& base);
    
    //render
    void RenderAll(std::ostream& out = std::cout);

private:
    //хранилище остановок с спроецированными координатами
    std::unordered_map<std::string_view, svg::Point> stop_coordinates_in_screen_format_;
    //хранилище остановок и автобосов которые через них проходят(в алф.порядке)
    std::unordered_map<std::string_view, std::set<std::string_view>> stops_which_includeded_in_bus_;
    //хранилище маршрута и его цвета заданного на карте отрисовки
    std::unordered_map<std::string_view, svg::Color*> color_base_;
    //хранилище имен маршрутов в отсортированном порядке
    std::vector<std::string_view> buses_prerender_;
    //хранилище имен остановок в отсортированном порядке
    std::vector<std::string_view> stops_prerender_;
    //база рендеринка графики
    Render render_;
    //база остановок и автобусов
    const Base& base_;
    //палитра
    const std::vector<svg::Color>& pallete_ = render_.ColorPallete();
    //счтетчик палитры
    size_t palette_counter_ = 0;
    //контейнер для рендеринга
    svg::Document container_;


    //ПРЕРЕНДЕРИНГ
    void LoadbaseStops();
    void PreRenderBuses();
    void PreRenderBusesNamesOnMap();
    void PrerenderStops();
    void PrerenderStopsNames();
      
   
    //круговой маршрут??
    bool IsCircledRoute(std::string_view name);
    //пустой маршрут??
    bool IsEmptyRoute(std::string_view name);
    //имеет 1 остановку???
    bool HasOneStop(std::string_view name);
    //в пререндеринге текста создает повторяющийся текст 
    svg::Text MakeTemplateText(std::string_view nameb, const svg::Point& needed_p);
    //задает атрибуты подложки
    void MakeUnderLayerAttrs(svg::Text& text);
};

/////////////////////////////////////////////////////
//                                                 //
//                     PREREND                     //
//                                                 //
//                                                 //
//                                                 //
//                                                 //
//                                                 //
//                                                 //
//                                                 //
/////////////////////////////////////////////////////

/////////////////////////////////////////////////////
//              КОНСТРУКТОР И ВЫВОД                //
/////////////////////////////////////////////////////
template<typename Render, typename Base>
Prerend<Render, Base>::Prerend(Render&& render, const Base& base) : render_(std::move(render)), base_(base) {
    LoadbaseStops();
    PreRenderBuses();
    PreRenderBusesNamesOnMap();
    PrerenderStops();
    PrerenderStopsNames();
}

template<typename Render, typename Base>
void Prerend<Render, Base>::RenderAll(std::ostream& out) {
    container_.Render(out);
}
/////////////////////////////////////////////////////
//             ФОРМИРОВАНИЕ БД                     //
/////////////////////////////////////////////////////
template<typename Render, typename Base>
void Prerend<Render,Base>::LoadbaseStops() {
    std::vector<geo::Coordinates> coord;
    for (const auto& i : base_.StopHashTable()) {
        auto buses_in_this_stop = base_.UniqueBusesOnNeededStop(i.second->name);
        if (!buses_in_this_stop.empty()) {
            stops_which_includeded_in_bus_.insert({ i.second->name, std::move(buses_in_this_stop) });
            coord.emplace_back(i.second->coord);
        }
    }
    SphereProjector proj(coord.begin(), coord.end(), render_.ScreenWidth(), render_.ScreenHeight(), render_.Padding());
    for (auto&& i : base_.StopHashTable()) {
        stop_coordinates_in_screen_format_.insert({ i.second->name , proj(i.second->coord) });
    }
}
template<typename Render, typename Base>
void Prerend<Render, Base>::PreRenderBuses() {

    for (const auto& [bus_name, bus] : base_.BusHashTable()) {
        buses_prerender_.push_back(bus_name);
    }
    std::sort(buses_prerender_.begin(), buses_prerender_.end(), [](std::string_view one, std::string_view two) {
        return one < two;
        });

    for (auto&& nameb : buses_prerender_) {
        svg::Polyline p;

        for (const auto& j : base_.GetNeededBus(nameb)->bus_root) {
            //^^^по всем остановка маршрута bus
            p.AddPoint(stop_coordinates_in_screen_format_.at(j->name));
        };

        p.SetStrokeColor(pallete_[palette_counter_])
            .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
            .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND)
            .SetStrokeWidth(render_.LineWidht()).SetFillColor("none");
        container_.Add(p);
        color_base_[nameb] = const_cast<svg::Color*>(&pallete_[palette_counter_]);
        ++palette_counter_;
        if (palette_counter_ == pallete_.size()) palette_counter_ = 0;

    }

}
template<typename Render, typename Base>
void Prerend<Render, Base>::PreRenderBusesNamesOnMap() {

    for (auto&& nameb : buses_prerender_) {
        //Если не пустой маршрут
        if (!IsEmptyRoute(nameb)) {
            //ищем в базе наш маршрут
            const Bus* needed = base_.GetNeededBus(nameb);
            //находим имя его первой остановки
            const std::string_view name_nrs = needed->bus_root[0]->name; //brs - needed root stop
            //находим screen- координаты этой остановки
            svg::Point needed_p = stop_coordinates_in_screen_format_.at(name_nrs);

            //Сделали два одинаковых текста по общим аттрибутам 
            svg::Text underlayer1 = MakeTemplateText(nameb, needed_p);
            svg::Text txt1 = MakeTemplateText(nameb, needed_p);
            //тексту мы придаем заливку
            txt1.SetFillColor(*color_base_.at(nameb));
            //подложке задаем ее доп.аттрибуты
            MakeUnderLayerAttrs(underlayer1);
            //ложим в наш рендер-контейнер
            container_.Add(underlayer1);
            container_.Add(txt1);

            //Если наш автобус имеет некольцевой маршрут: 
            if (!IsCircledRoute(nameb) && !HasOneStop(nameb)) {
                const std::string_view name_nrs2 = needed->bus_root[needed->bus_root.size() / 2]->name;
                /*                                                                 ^^^Внимание!!!
                   В данной реализации при добавлении остановок некольцевого маршрута [1,2,3]
                   программа делает [1,2,3,2,1]
                                         ^размер 5/2 = 2 - нужный индекс для отрисовки конечной;
                */
                //Если НЕ совпадают начальная и конечная остановки
                if (needed->bus_root[needed->bus_root.size() / 2] != needed->bus_root[0]) {
                    needed_p = stop_coordinates_in_screen_format_.at(name_nrs2);
                    svg::Text underlayer2 = MakeTemplateText(nameb, needed_p); ;
                    svg::Text txt2 = MakeTemplateText(nameb, needed_p);
                    txt2.SetFillColor(*color_base_.at(nameb));
                    MakeUnderLayerAttrs(underlayer2);
                    container_.Add(underlayer2);
                    container_.Add(txt2);
                }
            }
        }
    }

}


template<typename Render, typename Base>
void Prerend<Render, Base>::PrerenderStops() {

    for (const auto& [stop_name, bus] : stops_which_includeded_in_bus_) {
        stops_prerender_.push_back(stop_name);
    }
    std::sort(stops_prerender_.begin(), stops_prerender_.end(), [](std::string_view one, std::string_view two) {
        return one < two;
        });

    for (auto&& i : stops_prerender_) {
        svg::Circle cir;
        cir.SetCenter(stop_coordinates_in_screen_format_.at(i)).SetRadius(render_.StopRadius()).SetFillColor("white");
        container_.Add(cir);
    }
}


template<typename Render, typename Base>
void Prerend<Render, Base>::PrerenderStopsNames() {

    for (auto&& i : stops_prerender_) {
        svg::Text txt;
        txt.SetPosition(stop_coordinates_in_screen_format_.at(i)).SetOffset(render_.StopLabelOffsetPoint()).SetFontSize(render_.StopLabelFont()).SetFontFamily("Verdana").SetData(std::string(i));

        svg::Text underlayer = txt;

        txt.SetFillColor("black");

        MakeUnderLayerAttrs(underlayer);

        container_.Add(underlayer);
        container_.Add(txt);
    }
}

/////////////////////////////////////////////////////
//              СЛУЖЕБНЫЕ ПОМОЩНИКИ                //
/////////////////////////////////////////////////////
template<typename Render, typename Base>
bool Prerend<Render, Base>::IsCircledRoute(std::string_view name) {
    return base_.GetNeededBus(name)->is_circled;
}


template<typename Render, typename Base>
bool Prerend<Render, Base>::IsEmptyRoute(std::string_view name) {
    return base_.GetNeededBus(name)->bus_root.empty();
}

template<typename Render, typename Base>
bool Prerend<Render, Base>::HasOneStop(std::string_view name) {
    return (base_.GetNeededBus(name)->bus_root.size() == 1);
}


template<typename Render, typename Base>
svg::Text Prerend<Render, Base>::MakeTemplateText(std::string_view nameb, const svg::Point& needed_p) {
    svg::Text t;
    t.SetPosition(needed_p).
        SetOffset(svg::Point{render_.BusLabX(), render_.BusLabY() }).
        SetFontSize(render_.BusLabelFont()).
        SetFontFamily("Verdana").
        SetFontWeight("bold").
        SetData(std::string(nameb));
    return t;
}


template<typename Render, typename Base>
void Prerend<Render, Base>::MakeUnderLayerAttrs(svg::Text& text) {
    text.SetFillColor(render_.UnderLayerColor()).
        SetStrokeColor(render_.UnderLayerColor()).
        SetStrokeWidth(render_.UnderLayerW()).
        SetStrokeLineCap(svg::StrokeLineCap::ROUND).
        SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
}

/////////////////////////////////////////////////////
//                                                 //
//                SPHERE PROJECTOR                 //
//                                                 //
//                                                 //
//                                                 //
//                                                 //
//                                                 //
//                                                 //
//                                                 //
/////////////////////////////////////////////////////
template <typename PointInputIt>
SphereProjector::SphereProjector(PointInputIt points_begin, PointInputIt points_end,
    double max_width, double max_height, double padding)
    : padding_(padding) //
{
    // Если точки поверхности сферы не заданы, вычислять нечего
    if (points_begin == points_end) {
        return;
    }

    // Находим точки с минимальной и максимальной долготой
    const auto [left_it, right_it] = std::minmax_element(
        points_begin, points_end,
        [](auto lhs, auto rhs) { return lhs.lng < rhs.lng; });
    min_lon_ = left_it->lng;
    const double max_lon = right_it->lng;

    // Находим точки с минимальной и максимальной широтой
    const auto [bottom_it, top_it] = std::minmax_element(
        points_begin, points_end,
        [](auto lhs, auto rhs) { return lhs.lat < rhs.lat; });
    const double min_lat = bottom_it->lat;
    max_lat_ = top_it->lat;

    // Вычисляем коэффициент масштабирования вдоль координаты x
    std::optional<double> width_zoom;
    if (!IsZero(max_lon - min_lon_)) {
        width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
    }

    // Вычисляем коэффициент масштабирования вдоль координаты y
    std::optional<double> height_zoom;
    if (!IsZero(max_lat_ - min_lat)) {
        height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
    }

    if (width_zoom && height_zoom) {
        // Коэффициенты масштабирования по ширине и высоте ненулевые,
        // берём минимальный из них
        zoom_coeff_ = std::min(*width_zoom, *height_zoom);
    }
    else if (width_zoom) {
        // Коэффициент масштабирования по ширине ненулевой, используем его
        zoom_coeff_ = *width_zoom;
    }
    else if (height_zoom) {
        // Коэффициент масштабирования по высоте ненулевой, используем его
        zoom_coeff_ = *height_zoom;
    }
}