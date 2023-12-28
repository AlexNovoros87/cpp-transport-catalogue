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
	//                                     ��������                                           //
	//                                                                                        //
	////////////////////////////////////////////////////////////////////////////////////////////
	
	//�������� �������
	void AddStation(const Stop& stop);

	//�������� �������
	void AddBus(std::string_view name, std::vector<std::string_view>&& sv);
 
    ////////////////////////////////////////////////////////////////////////////////////////////
    //                                                                                        //
    //                              �������� �������������                                    //
    //                                                                                        //
    ////////////////////////////////////////////////////////////////////////////////////////////
	
	//��������� ������������� ��������
	bool HasBus(std::string_view name) const;
	
	//��������� ������������� ���������
	bool HasStop(std::string_view name) const;

	
	////////////////////////////////////////////////////////////////////////////////////////////
	//                                                                                        //
	//                                    ������ � ��                                         //
	//                                                                                        //
	////////////////////////////////////////////////////////////////////////////////////////////
	
	//���������� ��������� �� ������ �������� �� hesh_buses_
	const Bus* GetNeededBus(std::string_view name) const;
	
    //���������� ������� ���������
	const NamesToStops&  StopHashTable() const;
	
	//���������� ������� ���������
	const NamesToBuses& BusHashTable() const;
	
	////////////////////////////////////////////////////////////////////////////////////////////
	//                                                                                        //
	//                                    ������                                              //
	//                                                                                        //
	////////////////////////////////////////////////////////////////////////////////////////////

	//���������� �������������� ��������� ��������� �� ����� ���������
	std::set<std::string_view> UniqueBusesOnNeededStop(std::string_view stop_name) const;

private:
	std::deque<Stop> stops_deque_;
	std::deque<Bus>  buses_deque_;
	NamesToStops hesh_stops_;
	NamesToBuses hesh_buses_;
    
}; 



