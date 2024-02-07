#include "transport_catalogue.h"
#include <string>
#include <string_view>
#include <vector>
#include <unordered_set>
#include <optional>
#include <iostream>

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

void TransportCatalogue::AddBus(const std::string& route, const vector<Stop*>& stops) {
    Bus bus;
    bus.route = route;
    for (Stop* stop : stops) {
        bus.stops_on_route.push_back(stop);
    }
    all_routes_.push_back(bus);
    busname_to_bus_[all_routes_.back().route] = &all_routes_.back();
    for (Stop* stop : stops) {
        stop_to_buses_[stop->name_of_stop].insert(FindBus(route));
    }
    
}

/*void TransportCatalogue::SetDistance(const std::string& stop_from, std::vector<std::pair<std::string, int>> distances) {

    for (auto [stop_to, distance] : distances) {
       
        std::pair<Stop*, Stop*> key = {stopname_to_stop_.at(stop_from), stopname_to_stop_.at(stop_to)};
        distances_[key] = distance;
        
    }
}
*/
    
void TransportCatalogue::SetDistance(TransportCatalogue::Stop* stop_from, TransportCatalogue::Stop* stop_to, int distance) {
    
        distances_[{stop_from, stop_to}] = distance; 
}

TransportCatalogue::Stop* TransportCatalogue::FindStop(std::string_view stop_name) const { 
    if (stopname_to_stop_.count(stop_name) == 0) return nullptr;
    return stopname_to_stop_.at(stop_name);
}

TransportCatalogue::Bus* TransportCatalogue::FindBus(std::string_view bus_name) const {
    if (busname_to_bus_.count(bus_name) == 0) return nullptr;
    return busname_to_bus_.at(bus_name);
}
    
int TransportCatalogue::GetDistance(Stop* stop_from, Stop* stop_to) const {
    int distance{};
    if (distances_.count({stop_from, stop_to})) {
        distance = distances_.at({stop_from, stop_to});
    } else {
       distance = distances_.at({stop_to, stop_from});
    }
    return distance;
}

optional<TransportCatalogue::BusInfo> TransportCatalogue::GetBusInfo(std::string bus) const {
    if (TransportCatalogue::FindBus(bus) == nullptr) return nullopt;
    BusInfo bus_info;
    bus_info.stops_on_route = (TransportCatalogue::FindBus(bus)->stops_on_route).size();
    unordered_set<Stop*> unique_stops;
    for (Stop* stop : TransportCatalogue::FindBus(bus)->stops_on_route) {
        unique_stops.insert(stop);        
    }
    bus_info.unique_stops_num = unique_stops.size();
    
    double route_length_geo {};
    int size = (TransportCatalogue::FindBus(bus)->stops_on_route).size();
    for (int i = 0; i < size - 1; ++i) { 
        route_length_geo += ComputeDistance(TransportCatalogue::FindBus(bus)->stops_on_route[i]->coordinates, TransportCatalogue::FindBus(bus)->stops_on_route[i+1]->coordinates);
        int distance = GetDistance(TransportCatalogue::FindBus(bus)->stops_on_route[i], TransportCatalogue::FindBus(bus)->stops_on_route[i+1]);        
        bus_info.route_length += distance;
    }
    bus_info.curvature = (double)bus_info.route_length / route_length_geo;
    
    return bus_info;
    
}

optional<TransportCatalogue::StopInfo> TransportCatalogue::GetStopInfo(std::string stop) const {
    if (TransportCatalogue::FindStop(stop) == nullptr) return nullopt;
    StopInfo stop_info;
    for (auto bus : stop_to_buses_.at(stop)) {
        stop_info.buses.insert(bus->route);
    }
    return stop_info;
     
}
    
    size_t TransportCatalogue::Hasher::operator()(const std::pair<Stop*, Stop*>& pair) const {
        
        return (size_t)std::hash<void*>{}(pair.first)*29 + (size_t)std::hash<void*>{}(pair.second);
    }
}