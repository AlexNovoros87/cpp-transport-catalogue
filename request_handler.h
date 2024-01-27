#pragma once


////////////////////////////////////
#include "transport_catalogue.h" //
#include "json_reader.h"         
#include"map_renderer.h"


class Request final : private WorkWithJson {
public:
    //����������
    Request(std::istream& ist = std::cin);
    //������������� �������
    void PrintArray(std::ostream& ost = std::cout) const;

private:
    json::Array requests_array_{};
    //������������ �������
    void MakeArrayJson();
    //�������� �����������
    std::string MakeMapNode() const;
    //���� ����� �� ��� �����������
    void muter() override {};

};


