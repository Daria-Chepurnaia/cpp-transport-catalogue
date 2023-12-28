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
    // Реализуйте самостоятельно
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
    
    int stops_on_route{};
    size_t unique_stops_num{};
    double route_length{};
    
    if (tansport_catalogue.GetBusInfo(req, stops_on_route, unique_stops_num, route_length)) {  
        output << to_string(stops_on_route) << " stops on route, "s 
               << to_string(unique_stops_num) << " unique stops, "s
               << setprecision(6) << route_length << " route length"s << endl; 
    } else {
        output << "not found"s << endl;
    }
}

void PrintStopInfo(const TransportCatalogue& tansport_catalogue, std::string &req, std::ostream& output){
    output << "Stop "s << req << ": "s;
    std::set<std::string_view> buses{};
    if (tansport_catalogue.GetStopInfo(req, buses)) {
        if (buses.empty()) {
            output << "no buses"s << endl;
        } else {
            output << "buses "s;
            bool first = true;
            for (auto bus : buses) {                
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