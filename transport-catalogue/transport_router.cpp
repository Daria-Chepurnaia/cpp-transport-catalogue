#include "transport_router.h"

using namespace std::literals;

TransportRouter::TransportRouter(const transport_catalogue::TransportCatalogue& catalogue, int bus_wait_time, double bus_velocity)
    : catalogue_(catalogue)
        , graph_(catalogue.GetStopsCount()*2)
        , bus_wait_time_(bus_wait_time)
        , bus_velocity_(bus_velocity) {
        stop_indexes_.reserve(catalogue.GetStopsCount()*2);
        int i=0;            
        for (auto stop : catalogue.GetStopNames()) {
            stop_to_vertex_id_[stop]=i;
            stop_indexes_.push_back(stop);
            stop_indexes_.push_back(stop); 
            i+=2;
        }            
    }
void TransportRouter::BuildGraph() {
    //creates edges for waiting on every stop
    edge_id_to_route_.reserve(stop_indexes_.size()/2);
    for (long unsigned int i = 0; i < stop_indexes_.size(); ++i) {
        graph_.AddEdge({i, ++i, (double)bus_wait_time_});
        edge_id_to_route_.push_back(std::nullopt);            
    }

    //creates edges for all stops       
    for(auto bus_string_view : catalogue_.GetAllBuses()) {
        auto bus = catalogue_.FindBus(bus_string_view);

        if (bus->is_round) {                
            BuildEdges(0, bus->stops_on_route.size(), bus, bus_string_view);  
        } else {                
            BuildEdges(0, bus->stops_on_route.size()/2+1,bus, bus_string_view);  
            BuildEdges(bus->stops_on_route.size()/2, bus->stops_on_route.size(), bus, bus_string_view); 
        }           
    }       
}

const graph::DirectedWeightedGraph<double>& TransportRouter::GetGraph() const {
    return graph_;
}

void TransportRouter::GetRoutesInfo(graph::Router<double>* router,
                                    const std::string& from, const std::string& to,
                                    double& total_weight, std::optional<std::vector<ActivityInfo>>& info) {
    auto route_info =  router->BuildRoute(stop_to_vertex_id_.at(from), stop_to_vertex_id_.at(to));
    if (!route_info) {           
        info = std::nullopt;          
    } else {           
        total_weight = route_info.value().weight;
        std::vector<ActivityInfo> info_result;

        for (const auto& edge : route_info.value().edges) {
            ActivityInfo activity;
            if (edge_id_to_route_.at(edge) == std::nullopt) {
                activity.type = "Wait"s;
                activity.stop_name = stop_indexes_.at(graph_.GetEdge(edge).from);
                activity.time = bus_wait_time_;
            } else {
                activity.type = "Bus"s;
                activity.bus_name = edge_id_to_route_.at(edge).value().first;
                activity.span_count = edge_id_to_route_.at(edge).value().second;
                activity.time = graph_.GetEdge(edge).weight;
            }
            info_result.push_back(activity);
        }
        info = info_result;            
    }
}

void TransportRouter::BuildEdges(int external_cycle_var, int max_stop, Bus* bus, std::string_view bus_string_view) {                            
    for (int i = external_cycle_var; i<max_stop; ++i) { 
        double cur_distance{};
        for (int j = i+1; j<max_stop; ++j) {                        
            if (stop_to_vertex_id_.at(bus->stops_on_route[i]->name_of_stop) != stop_to_vertex_id_.at(bus->stops_on_route[j]->name_of_stop)) {
                cur_distance+=(double)catalogue_.GetDistance(bus->stops_on_route[j-1], bus->stops_on_route[j]);
                graph_.AddEdge({stop_to_vertex_id_.at(bus->stops_on_route[i]->name_of_stop)+1,
                                stop_to_vertex_id_.at(bus->stops_on_route[j]->name_of_stop),
                                cur_distance / bus_velocity_});
                edge_id_to_route_.push_back(std::pair<std::string_view, double>{bus_string_view, std::abs(i-j)});
            }
        }
    }
}

