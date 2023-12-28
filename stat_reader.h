#pragma once

#include <iosfwd>
#include <string_view>


#include"input_reader.h"

class TransportCatalogue;
void ParseAndPrintStat(const TransportCatalogue& tansport_catalogue, std::string_view request, std::ostream& output);
