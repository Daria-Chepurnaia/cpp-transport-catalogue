#pragma once

#include "map_renderer.h"
#include "transport_catalogue.h"
#include "sstream"

class RequestHandler {
public:    
    RequestHandler(const transport_catalogue::TransportCatalogue& db, const renderer::MapRenderer& renderer) 
        : db_(db)
        , renderer_(renderer) {            
        }

    // Returns information about the route (Bus request)
    std::optional<BusInfo> GetBusStat(const std::string_view& bus_name) const;
    
    // Returns routes that pass through the stop
    std::set<std::string_view> GetBusesByStop(const std::string_view& stop_name) const;
    
    // Returns all routes
    std::set<std::string_view> GetAllRoutes();
    
    // Returns all routes with at least one stop in alphabetical order
    std::vector<Bus*> GetAllRoutesWithInfo();
    
    void RenderMap(std::ostringstream& out) const;

private:    
    const transport_catalogue::TransportCatalogue& db_;
    const renderer::MapRenderer& renderer_;
};
