#include "stat_reader.h"

void ParseAndPrintStat(const TransportCatalogue& tansport_catalogue, std::string_view request, std::ostream& output) {

    auto pos = request.find(' ');
    std::string_view req = request.substr(0, pos);
    std::string_view name = request.substr(pos + 1);

    //≈—À» «¿œ–Œ— - ¿¬“Œ¡”—
    if (HELP_STRUCTURES::REQUEST::IS_BUS(req)) {

        if (!tansport_catalogue.BusCountAgree(name)) {
            output << "Bus " << name << ": not found" << std::endl;
        }
        else {
            auto tmp = tansport_catalogue.GetNeededBus(name);
            output << "Bus " << name << ": " <<
                tmp->bus_root.size() << " stops on route, " <<
                tmp->unique_stops << " unique stops, " <<
                tmp->lenght << " route length" <<
                std::endl;
        }
         
    }

    //≈—À» «¿œ–Œ— - Œ—“¿ÕŒ¬ ¿
    if (HELP_STRUCTURES::REQUEST::IS_STOP_POINT(req)) {
       
        if (!tansport_catalogue.StopCountAgree(name)) {
            output << "Stop " << name << ": not found" << std::endl;
            return;
        }
        int count = 0;
        std::set<std::string_view> sv;

        for (auto i : tansport_catalogue.BusHashTable()) {
            for (auto j : i.second->bus_root) {
                if (j->name == name) {
                    sv.insert(i.second->name);
                    ++count; break;
                }
            }
        }
        if (count == 0) {
            output << "Stop " << name << ": no buses" << std::endl;
            return;
        }
        
        int limiter = 0;
        output << "Stop " << name << ": buses ";
        for (auto&& i : sv) {
            output << i;
            if (limiter == count - 1) break;
            output << ' ';
        }
        output << std::endl;
    }

}