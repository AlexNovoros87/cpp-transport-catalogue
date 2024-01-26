#include <iostream>
#include <string>
#include<sstream>
#include <string_view>
#include <fstream>
#include"E:/include/log_duration.h"

/////////////////////////////////
#include"json_reader.h"       ///
#include"request_handler.h"   ///
#include"map_renderer.h"      ///
/////////////////////////////////

int main() {
    using namespace std;

    ifstream ifs("T2.txt", ios::binary);
    
    WorkWithJson w(ifs);
    LOG_DURATION("work");
    w.PrintArray();
    
 }