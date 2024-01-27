#include"json_reader.h"


/////////////////////////////////////////////////////
//                                                 //
//                WORK WITH JSON                   //
//                                                 //
//                                                 //
//                                                 //
//                                                 //
//                                                 //
//                                                 //
//                                                 //
//                                                 //
//                                                 //
/////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
///                         ������������                          ///
/////////////////////////////////////////////////////////////////////

WorkWithJson::WorkWithJson() : doc_(json::Load(std::cin)) {
    LoadBase();
}

//���� ���������� ����� ������ ������� �����
WorkWithJson::WorkWithJson(std::istream& ist) : doc_(json::Load(ist)) {
    LoadBase();
}

/////////////////////////////////////////////////////////////////////
///                     ������ - ������ � ��                      ///
/////////////////////////////////////////////////////////////////////

const json::Node& WorkWithJson::RenderSettings() const {
    return render_settings_;
}

const TransportCatalogue& WorkWithJson::Catalogue() const {
    return cat_;
}

/////////////////////////////////////////////////////////////////////
///                     ��������  ��                              ///
/////////////////////////////////////////////////////////////////////

void WorkWithJson::LoadBase() {

    auto  load_print_requests = SortRequests();
    auto requests_to_add_ = std::move(std::get<0>(load_print_requests));
    requests_to_get_ = std::move(std::get<1>(load_print_requests));
    cat_ = ConstructCatalog(requests_to_add_);
    render_settings_ = std::move(std::get<2>(load_print_requests));
  //  MakeArrayJson();

}

std::tuple<json::Node, json::Node, json::Node> WorkWithJson::SortRequests() {
    json::Node n = std::move(doc_.GetRoot());
    //������� �� ����������
    json::Node array_base;
    //������� �� ������
    json::Node array_stat;
    //��������� �� "base_requests" � "stat_requests"
    json::Node render_settings;

    for (auto&& [key, value] : n.AsMap()) {
        if (key == "base_requests") array_base = std::move(value);
        else if (key == "stat_requests")  array_stat = std::move(value);
        else if (key == "render_settings") render_settings = std::move(value);
    }

    return { array_base,array_stat, render_settings };
}

TransportCatalogue WorkWithJson::ConstructCatalog(json::Node& nod) {
    //������� �� ���������� -  ��������
    std::vector<json::Node> add_bus_req;
    //������� �� ���������� -  ���������
    std::vector<json::Node> add_stop_req;

    //��������� "base_requests" �� Stops � Bus
    for (auto&& i : nod.AsArray()) {
        std::string_view type = i.AsMap().at("type").AsString();
        if (type == "Bus") add_bus_req.emplace_back(std::move(i));
        else if (type == "Stop") add_stop_req.emplace_back(std::move(i));
    }
    ////////////////////////////////////////////////////////////////////

    //��������� �������
    TransportCatalogue cat;

    //������������ ������� �� ���������� ���������
    //��������� ���������
    for (auto&& i : add_stop_req) {
        std::string_view name = i.AsMap().at("name").AsString();
        geo::Coordinates coord{ i.AsMap().at("latitude").AsDouble(), i.AsMap().at("longitude").AsDouble() };
        cat.AddStation({ std::string(name),coord });
    }
    //��������� ���������
    for (auto&& i : add_stop_req) {
        std::string_view name = i.AsMap().at("name").AsString();
        for (auto&& j : i.AsMap().at("road_distances").AsMap()) {
            cat.AddDistance(name, j.first, j.second.AsInt());
        }
    }
    ///////////////////////////////////////////////////////////////////////// 

    //lj,fdk
    for (auto&& i : add_bus_req) {
        std::string_view name = i.AsMap().at("name").AsString();
        bool round = i.AsMap().at("is_roundtrip").AsBool();
        std::vector<std::string_view> routes;
        int to_reserve = static_cast<int>(i.AsMap().at("stops").AsArray().size());

        if (to_reserve > 0 && !round) {
            routes.reserve(to_reserve * 2 - 1);
        }
        else routes.reserve(to_reserve);

        for (const auto& j : i.AsMap().at("stops").AsArray()) {
            routes.emplace_back(j.AsString());
        }
        if (!round) {
            int backcounter = to_reserve - 1;
            for (int j = backcounter - 1; j >= 0; --j) {
                routes.emplace_back(routes[j]);
            }
        }

        cat.AddBus(name, routes, round);
    }
    return cat;
}


/////////////////////////////////////////////////////
//                                                 //
//              GRAPHICS HELPER                    //
//                                                 //
//                                                 //
//                                                 //
//                                                 //
//                                                 //
//                                                 //
//                                                 //
/////////////////////////////////////////////////////

    ///////////////////////////////////////////////
    ///                  �����                  ///
    ///////////////////////////////////////////////
const svg::Color& RenderGraphics::UnderLayerColor() const {
    return underlayer_;
}

const std::vector<svg::Color>& RenderGraphics::ColorPallete() const {
    return color_palette_;
}

double RenderGraphics::ScreenWidth() const {
    return root_.AsMap().at("width").AsDouble();
}

double RenderGraphics::ScreenHeight() const {
    return root_.AsMap().at("height").AsDouble();
}

double RenderGraphics::Padding() const {
    return root_.AsMap().at("padding").AsDouble();
}

double RenderGraphics::LineWidht() const {
    return root_.AsMap().at("line_width").AsDouble();
}

double RenderGraphics::UnderLayerW() const {
    return root_.AsMap().at("underlayer_width").AsDouble();
}

///////////////////////////////////////////////
///              ���������                  ///
///////////////////////////////////////////////

RenderGraphics::RenderGraphics(const json::Node& nod) : root_(nod) {
    underlayer_ = ConvertColotToToSVG(root_.AsMap().at("underlayer_color"));
    CreatePalette();
}
void RenderGraphics::CreatePalette() {
    for (auto&& i : root_.AsMap().at("color_palette").AsArray()) {
        color_palette_.emplace_back(ConvertColotToToSVG(i));
    }
}
svg::Color RenderGraphics::ConvertColotToToSVG(const json::Node& node) const {
    if (node.IsString()) return node.AsString();

    if (node.IsArray()) {
        if (node.AsArray().size() == 3) {
            return svg::Rgb{
                node.AsArray()[0].AsInt(),
                    node.AsArray()[1].AsInt(),
                    node.AsArray()[2].AsInt()
            };
        }
        if (node.AsArray().size() == 4) {
            return svg::Rgba{
                node.AsArray()[0].AsInt(),
                    node.AsArray()[1].AsInt(),
                    node.AsArray()[2].AsInt(),
                    node.AsArray()[3].AsDouble()
            };
        }

    }
    return std::monostate{};
}


///////////////////////////////////////////////
///              ��������                   ///
///////////////////////////////////////////////
int RenderGraphics::BusLabelFont() const {
    return root_.AsMap().at("bus_label_font_size").AsInt();
}

svg::Point RenderGraphics::BusLabelOffsetPoint() const {
    return {
        root_.AsMap().at("bus_label_offset").AsArray()[0].AsDouble() ,
        root_.AsMap().at("bus_label_offset").AsArray()[1].AsDouble()
    };
}

double RenderGraphics::BusLabX() const {
    return root_.AsMap().at("bus_label_offset").AsArray()[0].AsDouble();
}

double RenderGraphics::BusLabY() const {
    return root_.AsMap().at("bus_label_offset").AsArray()[1].AsDouble();
}

std::pair<double, double> RenderGraphics::BusLabelOffsetPair() const {
    return {
        root_.AsMap().at("bus_label_offset").AsArray()[0].AsDouble() ,
        root_.AsMap().at("bus_label_offset").AsArray()[1].AsDouble()
    };
}
///////////////////////////////////////////////
///              ���������                  ///
///////////////////////////////////////////////


int RenderGraphics::StopLabelFont() const {
    return root_.AsMap().at("stop_label_font_size").AsInt();
}

double RenderGraphics::StopRadius() const {
    return root_.AsMap().at("stop_radius").AsDouble();
}

svg::Point RenderGraphics::StopLabelOffsetPoint() const {
    return {
        root_.AsMap().at("stop_label_offset").AsArray()[0].AsDouble() ,
        root_.AsMap().at("stop_label_offset").AsArray()[1].AsDouble()
    };
}

double RenderGraphics::StopLabX() const {
    return root_.AsMap().at("stop_label_offset").AsArray()[0].AsDouble();
}

double RenderGraphics::StopLabY() const {
    return root_.AsMap().at("stop_label_offset").AsArray()[1].AsDouble();
}

std::pair<double, double> RenderGraphics::StopLabelOffsetPair() const {
    return {
        root_.AsMap().at("stop_label_offset").AsArray()[0].AsDouble() ,
        root_.AsMap().at("stop_label_offset").AsArray()[1].AsDouble()
    };
}




