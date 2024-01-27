#pragma once


////////////////////////////////////
#include "transport_catalogue.h" //
#include "json_reader.h"         
#include"map_renderer.h"


class Request final : private WorkWithJson {
public:
    //констуктор
    Request(std::istream& ist = std::cin);
    //распечатывает запросы
    void PrintArray(std::ostream& ost = std::cout) const;

private:
    json::Array requests_array_{};
    //обрабатывает запросы
    void MakeArrayJson();
    //рендерит изображение
    std::string MakeMapNode() const;
    //чтоб класс не был абстрактным
    void muter() override {};

};


