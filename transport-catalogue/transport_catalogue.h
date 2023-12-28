#pragma once

#include "geo.h"
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <string_view>
#include <vector>
#include <set>

namespace transport_catalogue{
class TransportCatalogue {
	
    public:
         struct Stop {
            std::string name_of_stop;
            Coordinates coordinates;
        };

        struct Bus {
            std::string route;
            std::vector<Stop*> stops_on_route;
        };
        void AddStop(const std::string& name, const Coordinates& coordinates);
        void AddBus(const std::string& route, const std::vector<std::string_view> stops);
        Stop* FindStop(std::string_view stop_name) const;
        Bus* FindBus(std::string_view bus_name) const;
    
        bool GetBusInfo(std::string bus, int &stops_on_route, size_t &unique_stops, double &route_length) const;
        bool GetStopInfo(std::string stop, std::set<std::string_view> &buses) const;
    
    private:       
    
        std::deque<Stop> stops_;
        std::unordered_map<std::string_view, Stop*> stopname_to_stop_;
        std::deque<Bus> all_routes_;
        std::unordered_map<std::string_view, Bus*> busname_to_bus_;
        std::unordered_map<std::string_view, std::unordered_set<std::string_view>> stop_to_buses_;
       
};
}