#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <string>
#include <cassert>
#include <iterator>
#include <vector>

#include"transport_catalogue.h"

/////////////////////ПРОТОТИП ЗАПУСКА ПРОГРАММЫ///////////////////////////////////////////////////////////
void RunCatalogue(std::istream& stream = std::cin);
//////////////////////////////////////////////////////////////////////////////////////////////////////////



namespace HELP_STRUCTURES {

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
         std::vector<CommandDescription> commands_;
    };

   
   