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

    // Возвращает информацию о маршруте (запрос Bus)
    std::optional<BusInfo> GetBusStat(const std::string_view& bus_name) const;
    
    // Возвращает маршруты, проходящие через остановку
    std::set<std::string_view> GetBusesByStop(const std::string_view& stop_name) const;
    
    // Возвращает все маршруты
    std::set<std::string_view> GetAllRoutes();
    
    //возвращает все маршруты хотя бы с одной остановкой в порядке возрастания названия
    std::vector<Bus*> GetAllRoutesWithInfo();
    
    void RenderMap(std::ostringstream& out) const;

private:    
    const transport_catalogue::TransportCatalogue& db_;
    const renderer::MapRenderer& renderer_;
};



