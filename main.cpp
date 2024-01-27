#include <iostream>
#include <string>
#include<sstream>
#include <string_view>
#include <fstream>

/////////////////////////////////
#include"request_handler.h"   ///
/////////////////////////////////

int main() {

    std::ifstream ifs("Text.txt", std::ios::binary);
    
    Request js(ifs);


    MapRenderer mr(RenderGraphics(js.rend()), js.cat());
    mr.RenderAll();




}