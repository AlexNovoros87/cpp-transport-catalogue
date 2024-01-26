#include"json_reader.h"
class Render_Graphics;

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

const json::Array& WorkWithJson::RequestsJson() const {
    return requests_array_;
};

const json::Node& WorkWithJson::RenderSettings() const {
    return render_settings_;
}

const TransportCatalogue& WorkWithJson::Catalogue() const {
    return cat_;
}

void WorkWithJson::PrintArray(std::ostream& ost) const {
    int counter = 0;
    ost << "[";
    for (auto&& i : requests_array_) {
        std::visit(json::NodeVisiter{ost}, i.GetValue());
        ++counter;
        if (counter == static_cast<int>(requests_array_.size())) break;
        ost << ",\n";
    }
    ost << "]";
}

/////////////////////////////////////////////////////////////////////
///                     ��������� ��                              ///
/////////////////////////////////////////////////////////////////////

void WorkWithJson::LoadBase() {

    auto  load_print_requests = SortRequests();
    auto requests_to_add_ = std::move(std::get<0>(load_print_requests));
    requests_to_get_ = std::move(std::get<1>(load_print_requests));
    cat_ = ConstructCatalog(requests_to_add_);
    render_settings_ = std::move(std::get<2>(load_print_requests));
    MakeArrayJson();
  
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

    //��������� ��������
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
            for (int i = backcounter - 1; i >= 0; --i) {
                routes.emplace_back(routes[i]);
            }
        }
        cat.AddBus(name, routes, round);
    }
    return cat;
}

void WorkWithJson::MakeArrayJson() {
    if (requests_to_get_.IsArray()) {
      
        json::Array answers;
        requests_array_.clear();

        for (const auto& i : requests_to_get_.AsArray()) {
          
            std::string_view type_req = i.AsMap().at("type").AsString();
            json::Dict dictionary;
            int id = i.AsMap().at("id").AsInt();
            if (REQUEST::IS_BUS(type_req) || REQUEST::IS_STOP_POINT(type_req)) { 
                std::string_view request_name = i.AsMap().at("name").AsString();
                if (REQUEST::IS_BUS(type_req)) {
                    if (!cat_.HasBus(request_name)) {
                        dictionary.insert({ "request_id" ,  id });
                        dictionary.insert({ "error_message" , "not found" });
                    }
                    else {
                        auto tmp = cat_.GetNeededBus(request_name);

                        dictionary.insert({ "curvature" , static_cast<double>(tmp->road_length / tmp->length) });
                        dictionary.insert({ "request_id" , id });
                        dictionary.insert({ "route_length" , tmp->road_length });
                        dictionary.insert({ "stop_count" , static_cast<int>(tmp->bus_root.size()) });
                        dictionary.insert({ "unique_stop_count" , static_cast<int>(tmp->unique_stops) });
                    }
                }
                else if (REQUEST::IS_STOP_POINT(type_req)) {

                    if (!cat_.HasStop(request_name)) {
                        dictionary.insert({ "request_id" , id });
                        dictionary.insert({ "error_message" , "not found" });
                    }
                    else {
                        std::set<std::string_view> sv = cat_.UniqueBusesOnNeededStop(request_name);
                        json::Array stops;
                        for (auto&& q : sv) {
                            stops.emplace_back(std::string(q));
                        }
                        dictionary.insert({ "buses" , std::move(stops) });
                        dictionary.insert({ "request_id", id });
                    }
                }
               
               
            }
            else if (REQUEST::IS_MAP(type_req)) {
                json::Node nod(MakeMapNode());
                dictionary.insert({ "request_id", id });
                dictionary.insert({ "map", std::move(nod)});
            }
            requests_array_.emplace_back(std::move(dictionary));
        }

    }
}

//����������� �����
std::string WorkWithJson::MakeMapNode() const {
    Prerend render(Render_Graphics(RenderSettings()), this->cat_);
    std::ostringstream ostr;
    render.RenderAll(ostr);
    return ostr.str();
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
    const svg::Color& Render_Graphics::UnderLayerColor() const {
        return underlayer_;
    }
  
    const std::vector<svg::Color>& Render_Graphics::ColorPallete() const {
        return color_palette_;
    }
    
    double Render_Graphics::ScreenWidth() const {
        return root_.AsMap().at("width").AsDouble();
    }

    double Render_Graphics::ScreenHeight() const {
        return root_.AsMap().at("height").AsDouble();
    }

    double Render_Graphics::Padding() const {
        return root_.AsMap().at("padding").AsDouble();
    }

    double Render_Graphics::LineWidht() const {
        return root_.AsMap().at("line_width").AsDouble();
    }

    double Render_Graphics::UnderLayerW() const {
        return root_.AsMap().at("underlayer_width").AsDouble();
    }
    
    ///////////////////////////////////////////////
    ///              ���������                  ///
    ///////////////////////////////////////////////
    
    Render_Graphics::Render_Graphics(const json::Node& nod) : root_(nod) {
        underlayer_ = ConvertColotToToSVG(root_.AsMap().at("underlayer_color"));
        CreatePalette();
    }
    void Render_Graphics::CreatePalette() {
        for (auto&& i : root_.AsMap().at("color_palette").AsArray()) {
            color_palette_.emplace_back(ConvertColotToToSVG(i));
        }
    }
    svg::Color Render_Graphics::ConvertColotToToSVG(const json::Node& node) const {
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
    int Render_Graphics::BusLabelFont() const {
        return root_.AsMap().at("bus_label_font_size").AsInt();
    }
    
    svg::Point Render_Graphics::BusLabelOffsetPoint() const {
        return {
            root_.AsMap().at("bus_label_offset").AsArray()[0].AsDouble() ,
            root_.AsMap().at("bus_label_offset").AsArray()[1].AsDouble()
        };
    }

    double Render_Graphics::BusLabX() const {
        return root_.AsMap().at("bus_label_offset").AsArray()[0].AsDouble();
    }

    double Render_Graphics::BusLabY() const {
        return root_.AsMap().at("bus_label_offset").AsArray()[1].AsDouble();
    }
      
    std::pair<double, double> Render_Graphics::BusLabelOffsetPair() const {
        return {
            root_.AsMap().at("bus_label_offset").AsArray()[0].AsDouble() ,
            root_.AsMap().at("bus_label_offset").AsArray()[1].AsDouble()
        };
    }
    ///////////////////////////////////////////////
    ///              ���������                  ///
    ///////////////////////////////////////////////
    
    
    int Render_Graphics::StopLabelFont() const {
        return root_.AsMap().at("stop_label_font_size").AsInt();
    }

    double Render_Graphics::StopRadius() const {
        return root_.AsMap().at("stop_radius").AsDouble();
    }

    svg::Point Render_Graphics::StopLabelOffsetPoint() const {
        return {
            root_.AsMap().at("stop_label_offset").AsArray()[0].AsDouble() ,
            root_.AsMap().at("stop_label_offset").AsArray()[1].AsDouble()
        };
    }

    double Render_Graphics::StopLabX() const {
        return root_.AsMap().at("stop_label_offset").AsArray()[0].AsDouble();
    }

    double Render_Graphics::StopLabY() const {
        return root_.AsMap().at("stop_label_offset").AsArray()[1].AsDouble();
    }

    std::pair<double, double> Render_Graphics::StopLabelOffsetPair() const {
        return {
            root_.AsMap().at("stop_label_offset").AsArray()[0].AsDouble() ,
            root_.AsMap().at("stop_label_offset").AsArray()[1].AsDouble()
        };
    }

   


