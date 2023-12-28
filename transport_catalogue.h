#pragma once
#include<vector>
#include<unordered_map>
#include<unordered_set>
#include<string_view>
#include<deque>
#include<iostream>
#include<string>
#include<cassert>
#include<set>
#include"geo.h"


struct Stop {
	std::string name;
	Coordinates coord;
};

struct Bus {
	std::string name;
	std::vector<Stop*> bus_root;
	size_t unique_stops;
	double length;
};

class TransportCatalogue {

	using NamesToStops = std::unordered_map<std::string_view, Stop*>;
	using NamesToBuses = std::unordered_map<std::string_view, Bus*>;

public:

	////////////////////////////////////////////////////////////////////////////////////////////
	//                                                                                        //
	//                                     ДОБАВИТЬ                                           //
	//                                                                                        //
	////////////////////////////////////////////////////////////////////////////////////////////
	
	//Добавить станцию
	void AddStation(const Stop& stop);

	//Добавить маршрут
	void AddBus(std::string_view name, std::vector<std::string_view>&& sv);
 
    ////////////////////////////////////////////////////////////////////////////////////////////
    //                                                                                        //
    //                              ПРОВЕРКА СУЩЕСТВОВАНИЯ                                    //
    //                                                                                        //
    ////////////////////////////////////////////////////////////////////////////////////////////
	
	//Проверяет существования Автобуса
	bool HasBus(std::string_view name) const;
	
	//Проверяет существование остановки
	bool HasStop(std::string_view name) const;

	
	////////////////////////////////////////////////////////////////////////////////////////////
	//                                                                                        //
	//                                    ДОСТУП К БД                                         //
	//                                                                                        //
	////////////////////////////////////////////////////////////////////////////////////////////
	
	//Возвращает указатель на обьект Автобуса ИЗ hesh_buses_
	const Bus* GetNeededBus(std::string_view name) const;
	
    //Возвращает Таблицу Остановок
	const NamesToStops&  StopHashTable() const;
	
	//Возвращает таблицу Автобусов
	const NamesToBuses& BusHashTable() const;
	
	////////////////////////////////////////////////////////////////////////////////////////////
	//                                                                                        //
	//                                    ПРОЧЕЕ                                              //
	//                                                                                        //
	////////////////////////////////////////////////////////////////////////////////////////////

	//Возвращает отсортированый контейнер маршрутов по имени остановки
	std::set<std::string_view> UniqueBusesOnNeededStop(std::string_view stop_name) const;

private:
	std::deque<Stop> stops_deque_;
	std::deque<Bus>  buses_deque_;
	NamesToStops hesh_stops_;
	NamesToBuses hesh_buses_;
    
}; 



