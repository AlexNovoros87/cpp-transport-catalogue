#pragma once
#include<optional>
#include<unordered_map>
#include<string_view>

////////////////////////////////////
#include"transport_catalogue.h"/////
#include"request_handler.h"    /////
#include"json.h"               /////
#include"map_renderer.h"       /////
////////////////////////////////////

/////////////////////////////////////////////////////
//                                                 //
//                WORK WITH JSON                   //
//                                                 //
/////////////////////////////////////////////////////

class WorkWithJson {
public:

    WorkWithJson();
    //сюда передается поток откуда грузить джсон
    WorkWithJson(std::istream& ist);
    //сюда попадает поток куда печатать оставил просто чтоб было....
    void PrintArray(std::ostream& ost = std::cout) const;

    const json::Array& RequestsJson() const;
    const json::Node& RenderSettings() const;
    const TransportCatalogue& Catalogue() const;

private:

    json::Document doc_;
    TransportCatalogue cat_;
    json::Node requests_to_get_;
    json::Array requests_array_{};
    json::Node render_settings_{};
    //парсит документ на категории добавить в базу / получить информацию с базы
    //конструирует справочник
    void LoadBase();

    //////////////////////////////////////////////////////////////
                          //LOADBASE
    std::tuple<json::Node, json::Node, json::Node> SortRequests();
    //создает базу данных каталога
    TransportCatalogue ConstructCatalog(json::Node& nod);
    //создает полноценный массив типа json::array
    void MakeArrayJson();
    //////////////////////////////////////////////////////////////
    std::string MakeMapNode() const;
};

/////////////////////////////////////////////////////
//                                                 //
//                HELP WITH  GRAPHICS              //
//                                                 //
/////////////////////////////////////////////////////
/*
Этот класс предназначен для удобства возврата значений
Node& который связан с графическими аттрибутами....
 Цель - избавиться от подобных записей
 root_.AsMap().at("bus_label_offset").AsArray()[0].AsDouble() ,
 root_.AsMap().at("bus_label_offset").AsArray()[1].AsDouble()
*/

class Render_Graphics {
public:

    //Общее назначение
    Render_Graphics(const json::Node& nod);
    const svg::Color& UnderLayerColor() const;
    const std::vector<svg::Color>& ColorPallete() const;


    //Общее
    double ScreenWidth() const;
    double ScreenHeight() const;
    double Padding() const;
    double LineWidht() const;
    double UnderLayerW() const;


    //Все что Bus
    int BusLabelFont() const;
    svg::Point BusLabelOffsetPoint() const;
    std::pair<double, double> BusLabelOffsetPair() const;
    double BusLabX() const;
    double BusLabY() const;


    //Все что Stop
    double StopRadius() const;
    int StopLabelFont() const;
    svg::Point StopLabelOffsetPoint() const;
    std::pair<double, double> StopLabelOffsetPair() const;
    double StopLabX() const;
    double StopLabY() const;

    void DiagnosticPrintcheck() {


        std::cout << " ScreenWidth: " << ScreenWidth() << " ScreenHeight: " << ScreenHeight()
            << " Padding: " << Padding() << " LineWidth: " << LineWidht() << " Underlayer: " << UnderLayerColor();

        std::cout << R"(
///////////////////////////////////
bus
///////////////////////////////////

)";
    }


private:
    const json::Node& root_;
    svg::Color underlayer_;
    std::vector<svg::Color> color_palette_;

    //служебные
    void CreatePalette();
    svg::Color ConvertColotToToSVG(const json::Node& node) const;


};