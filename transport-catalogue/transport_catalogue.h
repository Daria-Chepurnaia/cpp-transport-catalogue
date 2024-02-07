#pragma once

#include "geo.h"
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <string_view>
#include <vector>
#include <set>
#include <optional>

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
    
        struct StopInfo {
            std::set<std::string_view> buses;
        };

        struct BusInfo {
            int stops_on_route{};
            size_t unique_stops_num{};
            int route_length{};
            double curvature{};            
        };
    
        void AddStop(const std::string& name, const Coordinates& coordinates);
        void AddBus(const std::string& route, const std::vector<Stop*>& stops);
        void SetDistance(TransportCatalogue::Stop* stop_from, TransportCatalogue::Stop* stop_to, int distance);
        Stop* FindStop(std::string_view stop_name) const;
        Bus* FindBus(std::string_view bus_name) const;
        int GetDistance(Stop* stop_from, Stop* stop_to) const;
    
        std::optional<BusInfo> GetBusInfo(std::string bus) const;
        std::optional<StopInfo> GetStopInfo(std::string stop) const;        
    
    private:       
        class Hasher {
            public:
                 size_t operator()(const std::pair<Stop*, Stop*>& pair) const; 
        };
        std::deque<Stop> stops_;
        std::unordered_map<std::string_view, Stop*> stopname_to_stop_;
        std::deque<Bus> all_routes_;
        std::unordered_map<std::string_view, Bus*> busname_to_bus_;
        std::unordered_map<std::string_view, std::unordered_set<Bus*>> stop_to_buses_;
        std::unordered_map<std::pair<Stop*, Stop*>, int, Hasher> distances_;
       
};
}