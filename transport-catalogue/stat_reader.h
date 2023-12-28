#pragma once

#include <iosfwd>
#include <string_view>

#include "transport_catalogue.h"

namespace transport_catalogue{
namespace output_processing {
void ParseAndPrintStat(const TransportCatalogue& tansport_catalogue, std::string_view request, std::ostream& output);
namespace details{
void PrintBusInfo(const TransportCatalogue& tansport_catalogue, std::string &req, std::ostream& output);
void PrintStopInfo(const TransportCatalogue& tansport_catalogue, std::string &req, std::ostream& output);
}
}
}