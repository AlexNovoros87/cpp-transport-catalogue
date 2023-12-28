 #include "transport_catalogue.h"

  ////////////////////////////////////////////////////////////////////////////////////////////
  //                                                                                        //
  //                                     днаюбхрэ                                           //
  //                                                                                        //
  ////////////////////////////////////////////////////////////////////////////////////////////


void TransportCatalogue::AddStation(const Stop& stop) {
	stops_deque_.emplace_back(stop);
	hesh_stops_[stops_deque_.back().name] = &stops_deque_[stops_deque_.size() - 1];
}


////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
//                              опнбепйю ясыеярбнбюмхъ                                    //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////


bool TransportCatalogue::BusCountAgree(std::string_view name) const {
	return(hesh_buses_.count(name) > 0);
}

bool TransportCatalogue::StopCountAgree(std::string_view name) const {
	return (hesh_stops_.count(name) > 0);
}


////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
//                                    днярсо й ад                                         //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

const Bus* TransportCatalogue::GetNeededBus(std::string_view name) const {
	assert(BusCountAgree(name) == true);
	return hesh_buses_.at(name);
}

TransportCatalogue::Hesh_Stops TransportCatalogue::StopHashTable() const {
	return hesh_stops_;
}

TransportCatalogue::Hesh_Buses TransportCatalogue::BusHashTable() const {
	return hesh_buses_;
}