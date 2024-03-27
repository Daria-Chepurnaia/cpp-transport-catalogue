#pragma once

#include "router.h"
#include "transport_catalogue.h"
#include <memory>

struct ActivityInfo {
    std::string type;
    double time;
    std::string_view stop_name;
    std::string_view bus_name;
    int span_count;    
};

struct RouteReqInfo {
    std::optional<std::vector<ActivityInfo>> route_info;
    double total_time;
};

class TransportRouter {
public:
    TransportRouter(const transport_catalogue::TransportCatalogue& catalogue, int bus_wait_time, double bus_velocity);    
    const graph::DirectedWeightedGraph<double>& GetGraph() const;
    RouteReqInfo GetRoutesInfo(const std::string& from, const std::string& to);    
    
private:
    const transport_catalogue::TransportCatalogue& catalogue_;
    graph::DirectedWeightedGraph<double> graph_;
    std::vector<std::string_view>stop_indexes_;
    std::unordered_map<std::string_view, long unsigned int>stop_to_vertex_id_;
    //stores name of the route and number of stops for each edge
    std::vector<std::optional<std::pair<std::string_view, int>>>edge_id_to_route_;
    int bus_wait_time_;
    double bus_velocity_;
    std::unique_ptr<graph::Router<double>> router_;
    
    void BuildGraph();
    void BuildEdges(int external_cycle_var, int max_stop, Bus* bus, std::string_view bus_string_view);
};
