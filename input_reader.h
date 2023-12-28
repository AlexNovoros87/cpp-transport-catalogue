#pragma once
#include <string>
#include <string_view>
#include <vector>
#include "transport_catalogue.h"


namespace HELP_STRUCTURES {

    //��������� "�������� ����" 
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
        // ����������, ������ �� ������� (���� command ��������)
        explicit operator bool() const {
            return !command.empty();
        }

        bool operator!() const {
            return !operator bool();
        }

        std::string command;      // �������� �������
        std::string id;           // id �������� ��� ���������
        std::string description;  // ��������� �������
    };


    class InputReader {
    public:
        /**
         * ������ ������ � ��������� CommandDescription � ��������� ��������� � commands_
         */
        void ParseLine(std::string_view line);



        /**
         * ��������� ������� ������������ ����������, ��������� ������� �� commands_
         */
        void ApplyCommands(TransportCatalogue& catalogue) const;

    private:
        std::vector<CommandDescription> commands_;
    };

   // std::vector<std::string_view> HELP_FUNCTIONS::Split(std::string_view string, char delim);

