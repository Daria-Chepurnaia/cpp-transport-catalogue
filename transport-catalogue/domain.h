#pragma once

#include "geo.h"

#include <string>
#include <string_view>
#include <set>
#include <vector>

struct Stop {
    std::string name_of_stop;
    geo::Coordinates coordinates;
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