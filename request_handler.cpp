#include "request_handler.h"

    Request::Request(std::istream& ist ) : WorkWithJson(ist) {
        MakeArrayJson();

    }

    void Request::PrintArray(std::ostream& ost ) const {
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


    void Request::MakeArrayJson() {
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
                    dictionary.insert({ "map", std::move(nod) });
                }
                requests_array_.emplace_back(std::move(dictionary));

            }

        }
    }

    std::string Request::MakeMapNode() const {
        MapRenderer render(RenderGraphics(RenderSettings()), this->cat_);
        std::ostringstream ostr;
        render.RenderAll(ostr);
        return ostr.str();
    }


