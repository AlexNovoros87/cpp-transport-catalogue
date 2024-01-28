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


/////////////////////
#include"domain.h"///
/////////////////////


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
	void AddBus(std::string_view name,const std::vector<std::string_view>& sv, bool is_circle_route);
 
   //Добавить дистанцию 
	void AddDistance(std::string_view point1, std::string_view point2, int len);
	 
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
	
   	//Возвращает указатель на обьект Остановки  ИЗ hesh_stops_
	const Stop* GetNeededStop(std::string_view name) const;
	
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

	//Возвращает фактическую дистанцию между точками
	int GetDistance(std::string_view point1, std::string_view point2) const;

private:
	class HasherStop {
	public:
    	 size_t operator() (const std::pair<Stop* ,Stop*> & _pair) const {
			 return hs_(_pair.first) * 37 + hs_(_pair.second) * 41;
		 }
	private:
		std::hash<const void*>hs_;
	};
		
	std::deque<Stop> stops_deque_;
	std::deque<Bus>  buses_deque_;
	NamesToStops hash_stops_;
	NamesToBuses hash_buses_;
	std::unordered_map<std::pair<Stop*,Stop*>, int, HasherStop> database_lengths_;
}; 