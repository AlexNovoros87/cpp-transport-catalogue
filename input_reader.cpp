#include "input_reader.h"
#include"stat_reader.h"

namespace HELP_FUNCTIONS {
    
    std::pair<std::string_view, double> OneWordSplit(std::string_view line, std::string_view delimeter = "to") {
        auto pos = line.find(delimeter);
        int digit = 0;
        std::from_chars(line.data(), line.data() + pos, digit);
        std::string_view text = line.substr(pos + delimeter.size());
        return { text, static_cast<double>(digit) };
    }
    
     /**
     * Парсит строку вида "10.123,  -30.1837" и возвращает пару координат (широта, долгота)
     */
    Coordinates ParseCoordinates(std::string_view str) {
        static const double nan = std::nan("");
        auto not_space = str.find_first_not_of(' ');
        auto comma = str.find(',');

        if (comma == str.npos) {
            return { nan, nan };
        }

        auto not_space2 = str.find_first_not_of(' ', comma + 1);
        double lat = std::stod(std::string(str.substr(not_space, comma - not_space)));
        double lng = std::stod(std::string(str.substr(not_space2)));
        return { lat, lng };
    }

    /**
     * Удаляет пробелы в начале и конце строки
     */
    std::string_view Trim(std::string_view string) {
        const auto start = string.find_first_not_of(' ');
        if (start == string.npos) {
            return {};
        }
        return string.substr(start, string.find_last_not_of(' ') + 1 - start);
    }

    /**
     * Разбивает строку string на n строк, с помощью указанного символа-разделителя delim
     */
    std::vector<std::string_view> Split(std::string_view string, char delim) {
        std::vector<std::string_view> result;

        size_t pos = 0;
        while ((pos = string.find_first_not_of(' ', pos)) < string.length()) {
            auto delim_pos = string.find(delim, pos);
            if (delim_pos == string.npos) {
                delim_pos = string.size();
            }
            if (auto substr = Trim(string.substr(pos, delim_pos - pos)); !substr.empty()) {
                result.push_back(substr);
            }
            pos = delim_pos + 1;
        }
         return result;
    }

    /**
     * Парсит маршрут.
     * Для кольцевого маршрута (A>B>C>A) возвращает массив названий остановок [A,B,C,A]
     * Для некольцевого маршрута (A-B-C-D) возвращает массив названий остановок [A,B,C,D,C,B,A]
     */
    std::vector<std::string_view> ParseRoute(std::string_view route) {
        if (route.find('>') != route.npos) {
            return Split(route, '>');
        }

        auto stops = Split(route, '-');
        std::vector<std::string_view> results(stops.begin(), stops.end());
        results.insert(results.end(), std::next(stops.rbegin()), stops.rend());
        return results;
    }

    CommandDescription ParseCommandDescription(std::string_view line) {
        auto colon_pos = line.find(':');
        if (colon_pos == line.npos) {
            return {};
        }

        auto space_pos = line.find(' ');
        if (space_pos >= colon_pos) {
            return {};
        }

        auto not_space = line.find_first_not_of(' ', space_pos);
        if (not_space >= colon_pos) {
            return {};
        }

        return { 
                std::string(line.substr(0, space_pos)),
                std::string(line.substr(not_space, colon_pos - not_space)),
                std::string(line.substr(colon_pos + 1)) 
        };
    }
}
    void InputReader::ParseLine(std::string_view line) {
        auto command_description = HELP_FUNCTIONS::ParseCommandDescription(line);
        if (command_description) {
            commands_.push_back(std::move(command_description));
        }
    }

    
   void InputReader::ApplyCommands([[maybe_unused]] TransportCatalogue& catalogue) const { 
       
        std::vector<CommandDescription> LOCAL_ADD_BUS;
        const int DO_NOT_CONTAINS_COORDINATES = 2;
        std::vector<std::pair<std::string,std::string>>  LOCAL_ENLARGED_STOP_REQUESTS;
        
        for (auto && i : commands_) {
            HELP_STRUCTURES::GoodViewCommandDescription tmp{ HELP_FUNCTIONS::Trim(i.command), HELP_FUNCTIONS::Trim(i.id), HELP_FUNCTIONS::Trim(i.description) };
             
            if (HELP_STRUCTURES::REQUEST::IS_STOP_POINT(tmp.type)) {
                auto splitted_decription = HELP_FUNCTIONS::Split(tmp.description, ',');
                if (HELP_STRUCTURES::REQUEST::IS_ENLARGED_STOP_REQUEST_ADD(splitted_decription)) {
                    catalogue.AddStation({ std::string(tmp.id), HELP_FUNCTIONS::ParseCoordinates(i.description) });
                    auto pos = i.description.find(splitted_decription[DO_NOT_CONTAINS_COORDINATES]);
                    LOCAL_ENLARGED_STOP_REQUESTS.emplace_back(std::make_pair(std::move(i.id), std::move(i.description.substr(pos))));
                }
                else {
                    catalogue.AddStation({ std::string(tmp.id), HELP_FUNCTIONS::ParseCoordinates(i.description) });
                }
            }
            
            else if (HELP_STRUCTURES::REQUEST::IS_BUS(tmp.type)) {
                LOCAL_ADD_BUS.emplace_back(i);
            }

        }

        for (auto && i : LOCAL_ENLARGED_STOP_REQUESTS) {
            auto parsed_all_distanses = HELP_FUNCTIONS::Split(i.second, ',');
            for (auto&& j : parsed_all_distanses) {
                auto distance_to = HELP_FUNCTIONS::OneWordSplit(j);
                catalogue.AddDistance( HELP_FUNCTIONS::Trim(i.first), HELP_FUNCTIONS::Trim(distance_to.first), distance_to.second);
            }
        }
  
        for (auto && i : LOCAL_ADD_BUS) {
            std::vector<std::string_view> parsed_routes(HELP_FUNCTIONS::ParseRoute(HELP_FUNCTIONS::Trim(i.description)));
            catalogue.AddBus(HELP_FUNCTIONS::Trim(i.id), std::move(parsed_routes));
        }
   }

    void RunCatalogue( std::istream& stream) {
        TransportCatalogue catalogue;
        int base_request_count;
            stream >> base_request_count >> std::ws;
            {
                InputReader reader;
                for (int i = 0; i < base_request_count; ++i) {
                    std::string line;
                    std::getline(stream, line);
                    reader.ParseLine(line);
                }
                reader.ApplyCommands(catalogue);
            }
           
            {
                int stat_request_count;
                stream >> stat_request_count >> std::ws;
                for (int i = 0; i < stat_request_count; ++i) {
                    std::string line;
                    std::getline(stream, line);
                    ParseAndPrintStat(catalogue, line, std::cout);
                }
            }
    
    }