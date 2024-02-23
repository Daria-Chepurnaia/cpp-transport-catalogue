#pragma once

#include "json.h"
#include "json_builder.h"
#include "transport_catalogue.h"
#include "geo.h"
#include "map_renderer.h"
#include <string>
#include <iostream>
#include <sstream>

using namespace json;
using namespace transport_catalogue;

using namespace std::literals;

class JSONReader {
public:
    JSONReader(Document doc)
        : base_reqs_(doc.GetRoot().AsMap().at("base_requests"s))
        , stat_reqs_(doc.GetRoot().AsMap().at("stat_requests"s))
        , render_settings_(doc.GetRoot().AsMap().at("render_settings"s))
    {        
    }
    
    void FillCatalogue(TransportCatalogue& catalogue);     
    Document MakeJSON(const TransportCatalogue& catalogue, std::ostringstream& out) const;    
    renderer::RenderSettings GetRenderSettings();
    std::map<std::string, bool> GetBusNameToRoundTrip();
    
private:
    void FillAllStops(TransportCatalogue& catalogue);
    void FillAllRoutes(TransportCatalogue& catalogue);
    void FillAllDistances(TransportCatalogue& catalogue);
    std::vector<std::string> ProcessRoute(json::Node req);    
    void FillStopReq(Dict& req_info, const std::optional<StopInfo>& stop_info) const;    
    void FillBusReq(Dict& req_info, const std::optional<BusInfo>& bus_info) const;    
    svg::Color ProcessColorNode(Node node);    
    std::vector<svg::Color> ProcessPaletteNode(Node node);
    
    Node base_reqs_;
    Node stat_reqs_;
    Node render_settings_;
};