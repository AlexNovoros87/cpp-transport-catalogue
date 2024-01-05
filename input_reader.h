#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <string>
#include <cassert>
#include <iterator>
#include <vector>
#include <charconv>
         
#include"transport_catalogue.h"

/////////////////////ПРОТОТИП ЗАПУСКА ПРОГРАММЫ///////////////////////////////////////////////////////////
void RunCatalogue(std::istream& stream = std::cin);
//////////////////////////////////////////////////////////////////////////////////////////////////////////



namespace HELP_STRUCTURES {

    const int VECTOR_SIZE_IF_ENLARGED_STOP_REQUEST_ADD = 3;
    const int VECTOR_SIZE_IF_SIMPLE_STOP_REQUEST_ADD = 2;
 
    struct StopRequestNonSimple {
        std::string_view name;
        Coordinates coord;
        std::vector<std::string_view> lengths;
    };
    
    //Структура "ХОРОШЕГО ВИДА" 
    struct GoodViewCommandDescription {
        std::string_view type;
        std::string_view id;
        std::string_view description;
    };

    struct REQUEST {
        static bool IS_BUS(std::string_view line) {
            return (line == "Bus");
        }
        static bool IS_STOP_POINT(std::string_view line) {
            return line == "Stop";
        }
        static bool IS_ENLARGED_STOP_REQUEST_ADD(const std::vector<std::string_view> &vctr) {
            return vctr.size() >= VECTOR_SIZE_IF_ENLARGED_STOP_REQUEST_ADD;
        }
    
        static bool IS_SIMPLE_STOP_REQUEST_ADD(const std::vector<std::string_view> & vctr) {
            return vctr.size() == VECTOR_SIZE_IF_SIMPLE_STOP_REQUEST_ADD;
        }
    };
}
    
struct CommandDescription {
        // Определяет, задана ли команда (поле command непустое)
        explicit operator bool() const {
            return !command.empty();
        }

        bool operator!() const {
            return !operator bool();
        }

        std::string command;      // Название команды
        std::string id;           // id маршрута или остановки
        std::string description;  // Параметры команды
    };


    class InputReader {
    public:
        /**
         * Парсит строку в структуру CommandDescription и сохраняет результат в commands_
         */
        void ParseLine(std::string_view line);
        /**
         * Наполняет данными транспортный справочник, используя команды из commands_
         */
        void ApplyCommands(TransportCatalogue& catalogue) const;

    private:
        mutable std::vector<CommandDescription> commands_;
    };

   
   