#pragma once
#include<cmath>
#include<algorithm>
#include<unordered_map>
#include<map>
#include<set>


///////////////////////
#include"json_reader.h"
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


class MapRenderer {
public:

    //конструктор
    MapRenderer(RenderGraphics && render, const TransportCatalogue & base);

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
    RenderGraphics render_;
    //база остановок и автобусов
    const TransportCatalogue & base_;
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
};