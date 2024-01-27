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
    // points_begin � points_end ������ ������ � ����� ��������� ��������� geo::Coordinates
    template <typename PointInputIt>
    SphereProjector(PointInputIt points_begin, PointInputIt points_end,
        double max_width, double max_height, double padding);

    // ���������� ������ � ������� � ���������� ������ SVG-�����������
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

    //�����������
    MapRenderer(RenderGraphics && render, const TransportCatalogue & base);

    //render
    void RenderAll(std::ostream& out = std::cout);

private:
    //��������� ��������� � ���������������� ������������
    std::unordered_map<std::string_view, svg::Point> stop_coordinates_in_screen_format_;
    //��������� ��������� � ��������� ������� ����� ��� ��������(� ���.�������)
    std::unordered_map<std::string_view, std::set<std::string_view>> stops_which_includeded_in_bus_;
    //��������� �������� � ��� ����� ��������� �� ����� ���������
    std::unordered_map<std::string_view, svg::Color*> color_base_;
    //��������� ���� ��������� � ��������������� �������
    std::vector<std::string_view> buses_prerender_;
    //��������� ���� ��������� � ��������������� �������
    std::vector<std::string_view> stops_prerender_;
    //���� ���������� �������
    RenderGraphics render_;
    //���� ��������� � ���������
    const TransportCatalogue & base_;
    //�������
    const std::vector<svg::Color>& pallete_ = render_.ColorPallete();
    //�������� �������
    size_t palette_counter_ = 0;
    //��������� ��� ����������
    svg::Document container_;


    //������������
    void LoadbaseStops();
    void PreRenderBuses();
    void PreRenderBusesNamesOnMap();
    void PrerenderStops();
    void PrerenderStopsNames();


    //�������� �������??
    bool IsCircledRoute(std::string_view name);
    //������ �������??
    bool IsEmptyRoute(std::string_view name);
    //����� 1 ���������???
    bool HasOneStop(std::string_view name);
    //� ������������� ������ ������� ������������� ����� 
    svg::Text MakeTemplateText(std::string_view nameb, const svg::Point& needed_p);
    //������ �������� ��������
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
    // ���� ����� ����������� ����� �� ������, ��������� ������
    if (points_begin == points_end) {
        return;
    }

    // ������� ����� � ����������� � ������������ ��������
    const auto [left_it, right_it] = std::minmax_element(
        points_begin, points_end,
        [](auto lhs, auto rhs) { return lhs.lng < rhs.lng; });
    min_lon_ = left_it->lng;
    const double max_lon = right_it->lng;

    // ������� ����� � ����������� � ������������ �������
    const auto [bottom_it, top_it] = std::minmax_element(
        points_begin, points_end,
        [](auto lhs, auto rhs) { return lhs.lat < rhs.lat; });
    const double min_lat = bottom_it->lat;
    max_lat_ = top_it->lat;

    // ��������� ����������� ��������������� ����� ���������� x
    std::optional<double> width_zoom;
    if (!IsZero(max_lon - min_lon_)) {
        width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
    }

    // ��������� ����������� ��������������� ����� ���������� y
    std::optional<double> height_zoom;
    if (!IsZero(max_lat_ - min_lat)) {
        height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
    }

    if (width_zoom && height_zoom) {
        // ������������ ��������������� �� ������ � ������ ���������,
        // ���� ����������� �� ���
        zoom_coeff_ = std::min(*width_zoom, *height_zoom);
    }
    else if (width_zoom) {
        // ����������� ��������������� �� ������ ���������, ���������� ���
        zoom_coeff_ = *width_zoom;
    }
    else if (height_zoom) {
        // ����������� ��������������� �� ������ ���������, ���������� ���
        zoom_coeff_ = *height_zoom;
    }
};