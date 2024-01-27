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
    //���� ���������� ����� ������ ������� �����
    WorkWithJson(std::istream& ist);
    //���� �������� ����� ���� �������� ������� ������ ���� ����....
    const json::Node& RenderSettings() const;
    const TransportCatalogue& Catalogue() const;

protected:

    //�������� ���� �� ����������� WorkWithJson
    virtual void muter() = 0;
    
    json::Document doc_;
    TransportCatalogue cat_;
    json::Node requests_to_get_{};
    
    json::Node render_settings_{};
    
    //������ �������� �� ��������� �������� � ���� / �������� ���������� � ����
    //������������ ����������
    void LoadBase();

    //////////////////////////////////////////////////////////////
                          //LOADBASE
    std::tuple<json::Node, json::Node, json::Node> SortRequests();
    //������� ���� ������ ��������
    TransportCatalogue ConstructCatalog(json::Node& nod);
};


/////////////////////////////////////////////////////
//                                                 //
//                HELP WITH  GRAPHICS              //
//                                                 //
/////////////////////////////////////////////////////
/*
���� ����� ������������ ��� �������� �������� ��������
Node& ������� ������ � ������������ �����������....
 ���� - ���������� �� �������� �������
 root_.AsMap().at("bus_label_offset").AsArray()[0].AsDouble() ,
 root_.AsMap().at("bus_label_offset").AsArray()[1].AsDouble()
*/

class RenderGraphics {
public:

    //����� ����������
    RenderGraphics(const json::Node& nod);
    const svg::Color& UnderLayerColor() const;
    const std::vector<svg::Color>& ColorPallete() const;


    //�����
    double ScreenWidth() const;
    double ScreenHeight() const;
    double Padding() const;
    double LineWidht() const;
    double UnderLayerW() const;


    //��� ��� Bus
    int BusLabelFont() const;
    svg::Point BusLabelOffsetPoint() const;
    std::pair<double, double> BusLabelOffsetPair() const;
    double BusLabX() const;
    double BusLabY() const;


    //��� ��� Stop
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

    //���������
    void CreatePalette();
    svg::Color ConvertColotToToSVG(const json::Node& node) const;

};