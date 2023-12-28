#include <iostream>
#include <string>
#include<fstream>
#include<sstream>
#include "input_reader.h"
#include "stat_reader.h"



int main() {
    using namespace std;

    TransportCatalogue catalogue;
    
    int base_request_count;
    std::cin >> base_request_count >> ws;

    {
        InputReader reader;
        for (int i = 0; i < base_request_count; ++i) {
            string line;
            getline(cin, line);
            reader.ParseLine(line);
        }
        reader.ApplyCommands(catalogue);
    }

    int stat_request_count;
    std::cin >> stat_request_count >> ws;
    for (int i = 0; i < stat_request_count; ++i) {
        std::string line;
        getline(cin, line);
        ParseAndPrintStat(catalogue, line, cout);
    }
    
}