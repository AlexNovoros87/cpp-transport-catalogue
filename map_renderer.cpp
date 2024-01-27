    #include"map_renderer.h"
    bool IsZero(double value) {
        return std::abs(value) < EPSILON;
    }

    svg::Point SphereProjector::operator()(geo::Coordinates coords) const {
        return {
            (coords.lng - min_lon_) * zoom_coeff_ + padding_,
            (max_lat_ - coords.lat) * zoom_coeff_ + padding_
        };
    }




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

    MapRenderer::MapRenderer(RenderGraphics&& render, const TransportCatalogue& base) : render_(std::move(render)), base_(base) {
        LoadbaseStops();
        PreRenderBuses();
        PreRenderBusesNamesOnMap();
        PrerenderStops();
        PrerenderStopsNames();
    }


    void MapRenderer::RenderAll(std::ostream& out) {
        container_.Render(out);
    }
    /////////////////////////////////////////////////////
    //             ФОРМИРОВАНИЕ БД                     //
    /////////////////////////////////////////////////////

    void MapRenderer::LoadbaseStops() {
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

    void MapRenderer::PreRenderBuses() {

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

    void MapRenderer::PreRenderBusesNamesOnMap() {

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


    void MapRenderer::PrerenderStops() {

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



    void MapRenderer::PrerenderStopsNames() {

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

    bool MapRenderer::IsCircledRoute(std::string_view name) {
        return base_.GetNeededBus(name)->is_circled;
    }



    bool MapRenderer::IsEmptyRoute(std::string_view name) {
        return base_.GetNeededBus(name)->bus_root.empty();
    }


    bool MapRenderer::HasOneStop(std::string_view name) {
        return (base_.GetNeededBus(name)->bus_root.size() == 1);
    }


    svg::Text MapRenderer::MakeTemplateText(std::string_view nameb, const svg::Point& needed_p) {
        svg::Text t;
        t.SetPosition(needed_p).
            SetOffset(svg::Point{render_.BusLabX(), render_.BusLabY() }).
            SetFontSize(render_.BusLabelFont()).
            SetFontFamily("Verdana").
            SetFontWeight("bold").
            SetData(std::string(nameb));
        return t;
    }



    void MapRenderer::MakeUnderLayerAttrs(svg::Text& text) {
        text.SetFillColor(render_.UnderLayerColor()).
            SetStrokeColor(render_.UnderLayerColor()).
            SetStrokeWidth(render_.UnderLayerW()).
            SetStrokeLineCap(svg::StrokeLineCap::ROUND).
            SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
    }
