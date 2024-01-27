#pragma once
#include<optional>
#include<unordered_map>
#include<string_view>

////////////////////////////////////
#include"transport_catalogue.h"///// 
#include"json.h"               /////
#include"svg.h"               /////
////////////////////////////////////

/////////////////////////////////////////////////////
//                                                 //
//                WORK WITH JSON                   //
//                                                 //
/////////////////////////////////////////////////////

class WorkWithJson {
public:

    WorkWithJson();
    //сюда передаетс€ поток откуда грузить джсон
    WorkWithJson(std::istream& ist);
    //сюда попадает поток куда печатать оставил просто чтоб было....
    const json::Node& RenderSettings() const;
    const TransportCatalogue& Catalogue() const;

protected:

    //заглушка чтоб не создавались WorkWithJson
    virtual void muter() = 0;
    
    json::Document doc_;
    TransportCatalogue cat_;
    json::Node requests_to_get_{};
    
    json::Node render_settings_{};
    
    //парсит документ на категории добавить в базу / получить информацию с базы
    //конструирует справочник
    void LoadBase();

    //////////////////////////////////////////////////////////////
                          //LOADBASE
    std::tuple<json::Node, json::Node, json::Node> SortRequests();
    //создает базу данных каталога
    TransportCatalogue ConstructCatalog(json::Node& nod);
};


/////////////////////////////////////////////////////
//                                                 //
//                HELP WITH  GRAPHICS              //
//                                                 //
/////////////////////////////////////////////////////
/*
Ётот класс предназначен дл€ удобства возврата значений
Node& который св€зан с графическими аттрибутами....
 ÷ель - избавитьс€ от подобных записей
 root_.AsMap().at("bus_label_offset").AsArray()[0].AsDouble() ,
 root_.AsMap().at("bus_label_offset").AsArray()[1].AsDouble()
*/

class RenderGraphics {
public:

    //ќбщее назначение
    RenderGraphics(const json::Node& nod);
    const svg::Color& UnderLayerColor() const;
    const std::vector<svg::Color>& ColorPallete() const;


    //ќбщее
    double ScreenWidth() const;
    double ScreenHeight() const;
    double Padding() const;
    double LineWidht() const;
    double UnderLayerW() const;


    //¬се что Bus
    int BusLabelFont() const;
    svg::Point BusLabelOffsetPoint() const;
    std::pair<double, double> BusLabelOffsetPair() const;
    double BusLabX() const;
    double BusLabY() const;


    //¬се что Stop
    double StopRadius() const;
    int StopLabelFont() const;
    svg::Point StopLabelOffsetPoint() const;
    std::pair<double, double> StopLabelOffsetPair() const;
    double StopLabX() const;
    double StopLabY() const;

private:
    const json::Node& root_;
    svg::Color underlayer_;
    std::vector<svg::Color> color_palette_;

    //служебные
    void CreatePalette();
    svg::Color ConvertColotToToSVG(const json::Node& node) const;

};