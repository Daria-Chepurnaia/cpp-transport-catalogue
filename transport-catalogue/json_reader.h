#pragma once

#include "json.h"
#include "json_builder.h"
#include "transport_catalogue.h"
#include "geo.h"
#include "map_renderer.h"
#include "transport_router.h"
#include <string>
#include <iostream>
#include <sstream>

using namespace json;
using namespace transport_catalogue;

using namespace std::literals;

class JSONReader {
public:
    JSONReader(Document doc)
        : base_reqs_(doc.GetRoot().AsDict().at("base_requests"s))
        , stat_reqs_(doc.GetRoot().AsDict().at("stat_requests"s))
        , render_settings_(doc.GetRoot().AsDict().at("render_settings"s))
        , routing_settings_(doc.GetRoot().AsDict().at("routing_settings"s))
    {        
    }
    
    void FillCatalogue(TransportCatalogue& catalogue);     
    Document MakeJSON(const TransportCatalogue& catalogue, std::ostringstream& out) const;    
    renderer::RenderSettings GetRenderSettings();
    std::map<std::string, bool> GetBusNameToRoundTrip();
    int GetBusWaitTime() const;
    double GetBusVelocity() const;
    void SetRouter(graph::Router<double>* r);
    void SetTransportRouter(TransportRouter* tr_r);
    
private:
    void FillAllStops(TransportCatalogue& catalogue);
    void FillAllRoutes(TransportCatalogue& catalogue);
    void FillAllDistances(TransportCatalogue& catalogue);
    std::vector<std::string> ProcessRoute(json::Node req);    
    void FillStopReq(Dict& req_info, const std::optional<StopInfo>& stop_info) const;    
    void FillBusReq(Dict& req_info, const std::optional<BusInfo>& bus_info) const; 
    void FillRouteReq(Dict& req_info, const std::optional<std::vector<ActivityInfo>>& route_info, double total_time) const;
    svg::Color ProcessColorNode(Node node);    
    std::vector<svg::Color> ProcessPaletteNode(Node node);
    
    
    Node base_reqs_;
    Node stat_reqs_;
    Node render_settings_;
    Node routing_settings_;
    graph::Router<double>* router_;  
    TransportRouter* tr_router_;
};