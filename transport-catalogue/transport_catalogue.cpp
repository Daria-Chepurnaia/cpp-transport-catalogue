#include "transport_catalogue.h"
#include <string>
#include <string_view>
#include <vector>
#include <unordered_set>


using namespace std;

namespace transport_catalogue{
void TransportCatalogue::AddStop(const std::string& name, const Coordinates& coordinates) {
    Stop stop;
    stop.name_of_stop = name;
    stop.coordinates = coordinates;
    stops_.push_back(stop);
    stopname_to_stop_[stops_.back().name_of_stop] = &stops_.back();
    stop_to_buses_[stops_.back().name_of_stop];
}

void TransportCatalogue::AddBus(const std::string& route, const vector<string_view> stops) {
    Bus bus;
    bus.route = route;
    for (const string_view& stop : stops) {
        bus.stops_on_route.push_back(stopname_to_stop_.at(stop));
    }
    all_routes_.push_back(bus);
    busname_to_bus_[all_routes_.back().route] = &all_routes_.back();
    for (string_view stop : stops) {
        stop_to_buses_[stop].insert(all_routes_.back().route);
    }
}

TransportCatalogue::Stop* TransportCatalogue::FindStop(std::string_view stop_name) const { 
    if (stopname_to_stop_.count(stop_name) == 0) return nullptr;
    return stopname_to_stop_.at(stop_name);
}

TransportCatalogue::Bus* TransportCatalogue::FindBus(std::string_view bus_name) const {
    if (busname_to_bus_.count(bus_name) == 0) return nullptr;
    return busname_to_bus_.at(bus_name);
}

bool TransportCatalogue::GetBusInfo(std::string bus, int &stops_on_route, size_t &unique_stops_num, double &route_length) const {
    if (TransportCatalogue::FindBus(bus) == nullptr) return false;
    stops_on_route = (TransportCatalogue::FindBus(bus)->stops_on_route).size();
    unordered_set<Stop*> unique_stops;
    for (Stop* stop : TransportCatalogue::FindBus(bus)->stops_on_route) {
        unique_stops.insert(stop);        
    }
    unique_stops_num = unique_stops.size();
    
    int size = (TransportCatalogue::FindBus(bus)->stops_on_route).size();
    for (int i = 0; i < size - 1; ++i) { 
        route_length += ComputeDistance(TransportCatalogue::FindBus(bus)->stops_on_route[i]->coordinates, TransportCatalogue::FindBus(bus)->stops_on_route[i+1]->coordinates);                
    }
    return true;
}

bool TransportCatalogue::GetStopInfo(std::string stop, std::set<std::string_view> &buses) const {
    if (TransportCatalogue::FindStop(stop) == nullptr) return false;
    for (auto bus : stop_to_buses_.at(stop)) {
        buses.insert(bus);
    }
    return true;
}
}