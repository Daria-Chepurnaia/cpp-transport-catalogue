#include "stat_reader.h"

#include <string>
#include <string_view>
#include <iomanip>
#include <set>

using namespace std;

namespace transport_catalogue {
namespace output_processing {
void ParseAndPrintStat(const TransportCatalogue& tansport_catalogue, std::string_view request,
                       std::ostream& output) {    
    size_t first_space = request.find(' ');
    size_t start = request.find_first_not_of(' ', first_space);
    size_t end = request.find_last_not_of(' ', request.size() - 1);   
    std::string req = (std::string)request.substr(start, end - start + 1);
    
    if (request[0] == 'B') {
        details::PrintBusInfo(tansport_catalogue, req, output);
    }
    if (request[0] == 'S') {
        details::PrintStopInfo(tansport_catalogue, req, output);
    }    
}
    
namespace details{
void PrintBusInfo(const TransportCatalogue& tansport_catalogue, std::string &req, std::ostream& output){
    output << "Bus "s << req << ": "s;
    
    auto bus_info = tansport_catalogue.GetBusInfo(req);    
    
    if (bus_info) {  
        output << to_string(bus_info.value().stops_on_route) << " stops on route, "s 
               << to_string(bus_info.value().unique_stops_num) << " unique stops, "s
               << setprecision(6) << bus_info.value().route_length << " route length, "s
               << bus_info.value().curvature << " curvature"s << endl; 
    } else {
        output << "not found"s << endl;
    }
}

void PrintStopInfo(const TransportCatalogue& tansport_catalogue, std::string &req, std::ostream& output){
    output << "Stop "s << req << ": "s;
    std::set<std::string_view> buses{};
    auto stop_info = tansport_catalogue.GetStopInfo(req);
    if (stop_info) {
        if (stop_info.value().buses.empty()) {
            output << "no buses"s << endl;
        } else {
            output << "buses "s;
            bool first = true;
            for (auto bus : stop_info.value().buses) {                
                if (first == true) {
                    output << bus;
                    first = false;
                } else {
                   output << " "s << bus;
                }                
            }
            output << endl;
        }
        
    } else {
        output << "not found"s << endl;
    }
}
}
}
}