#include "stat_reader.h"

void ParseAndPrintStat(const TransportCatalogue& tansport_catalogue, std::string_view request, std::ostream& output) {

    auto pos = request.find(' ');
    std::string_view req = request.substr(0, pos);
    std::string_view name = request.substr(pos + 1);

    //≈—À» «¿œ–Œ— - ¿¬“Œ¡”—
    if (HELP_STRUCTURES::REQUEST::IS_BUS(req)) {

        if (!tansport_catalogue.HasBus(name)) {
            output << "Bus " << name << ": not found" << std::endl;
        }
        else {
            auto tmp = tansport_catalogue.GetNeededBus(name);
            output << "Bus " << name << ": " <<
                tmp->bus_root.size() << " stops on route, " <<
                tmp->unique_stops << " unique stops, " <<
                tmp->length << " route length" <<
                std::endl;
        }
         
    }

    //≈—À» «¿œ–Œ— - Œ—“¿ÕŒ¬ ¿
    if (HELP_STRUCTURES::REQUEST::IS_STOP_POINT(req)) {
       
        if (!tansport_catalogue.HasStop(name)) {
            output << "Stop " << name << ": not found" << std::endl;
            return;
        }
        
        std::set<std::string_view> sv = tansport_catalogue.UniqueBusesOnNeededStop(name);

        if (sv.empty()) {
            output << "Stop " << name << ": no buses" << std::endl;
            return;
        }
        
        output << "Stop " << name << ": buses ";
        for (auto&& i : sv) {
            output << i << ' ';
        }
        output << std::endl;
    }

}


