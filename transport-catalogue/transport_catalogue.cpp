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

void TransportCatalogue::AddDistances(const std::string& stop_from, std::vector<std::pair<std::string, int>> distances) {

    for (auto [stop_to, distance] : distances) {
       
        std::pair<Stop*, Stop*> key = {stopname_to_stop_.at(stop_from), stopname_to_stop_.at(stop_to)};
        distances_[key] = distance;
        
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
        int distance{};
        if (distances_.count({TransportCatalogue::FindBus(bus)->stops_on_route[i], TransportCatalogue::FindBus(bus)->stops_on_route[i+1]})) {
            distance = distances_.at({TransportCatalogue::FindBus(bus)->stops_on_route[i], TransportCatalogue::FindBus(bus)->stops_on_route[i+1]});
        } else {
            distance = distances_.at({TransportCatalogue::FindBus(bus)->stops_on_route[i+1], TransportCatalogue::FindBus(bus)->stops_on_route[i]});
        }
        bus_info.route_length += distance;
    }
    bus_info.curvature = (double)bus_info.route_length / route_length_geo;
    
    return bus_info;
    
}

optional<TransportCatalogue::StopInfo> TransportCatalogue::GetStopInfo(std::string stop) const {
    if (TransportCatalogue::FindStop(stop) == nullptr) return nullopt;
    StopInfo stop_info;
    for (auto bus : stop_to_buses_.at(stop)) {
        stop_info.buses.insert(bus);
    }
    return stop_info;
     
}
    
    size_t TransportCatalogue::Hasher::operator()(const std::pair<Stop*, Stop*>& pair) const {
        
        return (size_t)std::hash<void*>{}(pair.first)*29 + (size_t)std::hash<void*>{}(pair.second);
    }
}