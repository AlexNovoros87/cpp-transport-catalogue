#pragma once

#include <iosfwd>
#include <string_view>
#include<set>
#include"input_reader.h"

void ParseAndPrintStat(const TransportCatalogue& tansport_catalogue, std::string_view request, std::ostream& output);
