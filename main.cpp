#include <iostream>
#include <string>
#include<sstream>
#include <sstream>
#include <fstream>
#include <cmath>
#include"input_reader.h"

#define DEBUG 1
#ifdef DEBUG
#include"E:/include/log_duration.h"
#include"E:/include/overflow.h"
#endif

int main() {
  
#ifdef DEBUG 
    
    using namespace std;

    ostringstream oss;
    ifstream ifs("T.txt");

    while (ifs.good()) {
        string line;
        getline(ifs, line);
        oss << line << endl;
    }
    istringstream iss(oss.str());
    LOG_DURATION("TEST");
    RunCatalogue(iss);
#else
    RunCatalogue();
#endif

}