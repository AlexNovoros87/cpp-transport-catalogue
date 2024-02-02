 #include "transport_catalogue.h"

	  ////////////////////////////////////////////////////////////////////////////////////////////
	  //                                                                                        //
	  //                                     днаюбхрэ                                           //
	  //                                                                                        //
	  ////////////////////////////////////////////////////////////////////////////////////////////


	void TransportCatalogue::AddStation(const Stop& stop) {
		stops_deque_.emplace_back(stop);
		hash_stops_[stops_deque_.back().name] = &stops_deque_[stops_deque_.size() - 1];
	}


	void TransportCatalogue::AddBus(std::string_view name,const std::vector<std::string_view>& sv, bool is_circle_route) {
		std::vector<Stop*> stops;
	
		stops.reserve(sv.size());
		std::unordered_set<std::string_view> unique_counter;
		double total_lenght = 0.;
		int total_road_length = 0;
	
		for (auto i : sv) {
			assert(HasStop(i) == true);
			stops.emplace_back(hash_stops_.at(i));
			unique_counter.insert(i);
		}

		for (size_t i = 0; i + 1 < stops.size(); ++i) {
			double lng = ComputeDistance(stops[i]->coord, stops[i + 1]->coord);
			if (database_lengths_.count({ stops[i], stops[i + 1] }) > 0) {
				total_road_length += GetDistance(stops[i]->name, stops[i + 1]->name);
			
			}
			else if (database_lengths_.count({ stops[i+1], stops[i] }) > 0) {
				total_road_length += GetDistance(stops[i + 1]->name, stops[i]->name);
			}
			total_lenght += lng;
		}
		buses_deque_.push_back({ std::string(name), std::move(stops), unique_counter.size(), total_lenght, total_road_length , is_circle_route });
		hash_buses_[buses_deque_[buses_deque_.size() - 1].name] = &buses_deque_[buses_deque_.size() - 1];
	}


	void TransportCatalogue::AddDistance(std::string_view point1, std::string_view point2, int len) {
		if (HasStop(point1) && HasStop(point2)) {
			database_lengths_[{hash_stops_.at(point1), hash_stops_.at(point2)}] = len;
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////
	//                                                                                        //
	//                              опнбепйю ясыеярбнбюмхъ                                    //
	//                                                                                        //
	////////////////////////////////////////////////////////////////////////////////////////////

	bool TransportCatalogue::HasBus(std::string_view name) const {
		return(hash_buses_.count(name) > 0);
	}

	bool TransportCatalogue::HasStop(std::string_view name) const {
		return (hash_stops_.count(name) > 0);
	}

	////////////////////////////////////////////////////////////////////////////////////////////
	//                                                                                        //
	//                                    днярсо й ад                                         //
	//                                                                                        //
	////////////////////////////////////////////////////////////////////////////////////////////

	const Bus* TransportCatalogue::GetNeededBus(std::string_view name) const {
		assert(HasBus(name) == true);
		return hash_buses_.at(name);
	}

	const Stop* TransportCatalogue::GetNeededStop(std::string_view name) const {
		assert(HasStop(name) == true);
		return hash_stops_.at(name);
	}

	const TransportCatalogue::NamesToStops& TransportCatalogue::StopHashTable() const {
		return hash_stops_;
	}

	const TransportCatalogue::NamesToBuses& TransportCatalogue::BusHashTable() const {
		return hash_buses_;
	}

	////////////////////////////////////////////////////////////////////////////////////////////
	//                                                                                        //
	//                                    опнвее                                              //
	//                                                                                        //
	////////////////////////////////////////////////////////////////////////////////////////////

	std::set<std::string_view> TransportCatalogue::UniqueBusesOnNeededStop(std::string_view stop_name) const {
		std::set<std::string_view> storage;
		for (auto i : BusHashTable()) {
			for (auto j : i.second->bus_root) {
				if (j->name == stop_name) {
					storage.insert(i.second->name);
					break;
				}
			}
		}
		return storage;
	}

	int TransportCatalogue::GetDistance(std::string_view point1, std::string_view point2) const {
		if (database_lengths_.count({ hash_stops_.at(point1), hash_stops_.at(point2) }) > 0) {
			return database_lengths_.at({ hash_stops_.at(point1), hash_stops_.at(point2) });
		}
		return 0;
	}