#pragma once
#include<vector>
#include<unordered_map>
#include<unordered_set>
#include<string_view>
#include<deque>
#include<iostream>
#include<string>
#include<cassert>
#include"geo.h"


struct Stop {
	std::string name;
	Coordinates coord;
};

struct Bus {
	std::string name;
	std::vector<Stop*> bus_root;
	size_t unique_stops;
	double lenght;
};

class TransportCatalogue {

    using Hesh_Stops = std::unordered_map<std::string_view, Stop*>;
	using Hesh_Buses = std::unordered_map<std::string_view, Bus*>;

public:

	////////////////////////////////////////////////////////////////////////////////////////////
	//                                                                                        //
	//                                     ДОБАВИТЬ                                           //
	//                                                                                        //
	////////////////////////////////////////////////////////////////////////////////////////////
	
	//Добавить станцию
	void AddStation(const Stop& stop);

	//Добавить маршрут
	template<typename FOO>
	void AddBus(std::string_view name, std::string_view route, FOO func);
 
    ////////////////////////////////////////////////////////////////////////////////////////////
    //                                                                                        //
    //                              ПРОВЕРКА СУЩЕСТВОВАНИЯ                                    //
    //                                                                                        //
    ////////////////////////////////////////////////////////////////////////////////////////////
	
	
	
	//Проверяет существования Автобуса
	bool BusCountAgree(std::string_view name) const;
	
	//Проверяет существование остановки
	bool StopCountAgree(std::string_view name) const;

	
	////////////////////////////////////////////////////////////////////////////////////////////
	//                                                                                        //
	//                                    ДОСТУП К БД                                         //
	//                                                                                        //
	////////////////////////////////////////////////////////////////////////////////////////////
	
	//Возвращает указатель на обьект Автобуса ИЗ hesh_buses_
	const Bus* GetNeededBus(std::string_view name) const;
	
    //Возвращает Таблицу Остановок
	Hesh_Stops StopHashTable() const;
	
	//Возвращает таблицу Автобусов
	Hesh_Buses BusHashTable() const;
	
private:
	std::deque<Stop> stops_deque_;
	std::deque<Bus>  buses_deque_;
	Hesh_Stops hesh_stops_;
	Hesh_Buses hesh_buses_;
    
}; 

    ////////////////////////////////////////////////////////////////////////////////////////////
	//                                                                                        //
	//                                     ДОБАВИТЬ                                           //
	//                                                                                        //
	////////////////////////////////////////////////////////////////////////////////////////////

template<typename FOO>
void TransportCatalogue::AddBus(std::string_view name, std::string_view route, FOO func) {

    std::vector<std::string_view> sv = func(route);
	std::vector<Stop*> stops;
	stops.reserve(sv.size());
	std::unordered_set<std::string_view> unique_counter;
	double total_lenght = 0.;

	for (auto i : sv) {
		assert(StopCountAgree(i) == true);
		stops.emplace_back(hesh_stops_.at(i));
		unique_counter.insert(i);
		
	}
	for (int i = 0; i + 1 < stops.size(); ++i) {
		double lng = ComputeDistance(stops[i]->coord, stops[i + 1]->coord);
		total_lenght += lng;
	}
	buses_deque_.push_back({ std::string(name), std::move(stops), unique_counter.size(), total_lenght });
	hesh_buses_[buses_deque_[buses_deque_.size() - 1].name] = &buses_deque_[buses_deque_.size() - 1];
}