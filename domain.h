#pragma once
#include<string>
#include<vector>

//////////////////
#include"geo.h"///
//////////////////

struct Stop {
	std::string name;
	geo::Coordinates coord;
};

struct Bus {
	std::string name;
	std::vector<Stop*> bus_root;
	size_t unique_stops;
	double length;
	int road_length;
	bool is_circled;
};
    
struct REQUEST {
	static bool IS_MAP(std::string_view line) {
		return (line == "Map");
	}
	static bool IS_BUS(std::string_view line) {
		return (line == "Bus");
	}
	static bool IS_STOP_POINT(std::string_view line) {
		return (line == "Stop");
	}
        
};

