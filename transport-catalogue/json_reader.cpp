#include "json_reader.h"

void JSONReader::FillCatalogue(TransportCatalogue& catalogue) {
    for (auto req : base_reqs_.AsArray()) {
        if (req.AsMap().at("type"s) == "Stop"s) {
            catalogue.AddStop(req.AsMap().at("name"s).AsString(), {req.AsMap().at("latitude"s).AsDouble(), req.AsMap().at("longitude"s).AsDouble()});
        }
    }
    
    for (auto req : base_reqs_.AsArray()) {        
        if (req.AsMap().at("type"s) == "Bus"s) {            
            std::vector<Stop*> stops_to_add;
            std::vector<std::string> stops = ProcessRoute(req);           
            for (auto stop : stops) {
                stops_to_add.push_back(catalogue.FindStop(stop));
            }            
            catalogue.AddBus(req.AsMap().at("name"s).AsString(), stops_to_add);            
        }
    }
   
    for (auto req : base_reqs_.AsArray()) {
        if (req.AsMap().at("type"s) == "Stop"s) {
            
            for (auto [stop_to, distance] : req.AsMap().at("road_distances"s).AsMap()) {
                catalogue.SetDistance(catalogue.FindStop(req.AsMap().at("name"s).AsString()), catalogue.FindStop(stop_to), distance.AsInt());
            } 
            
        }
    }    
}

Document JSONReader::MakeJSON(const TransportCatalogue& catalogue, std::ostringstream& out) {
    Array info;
    for (auto req : stat_reqs_.AsArray()) {
        Dict req_info;

        if (req.AsMap().at("type"s).AsString() == "Stop"s) {                
            std::optional<StopInfo> stop_info = catalogue.GetStopInfo(req.AsMap().at("name"s).AsString());
            //заполняет словарь req_info информацией об остановке
            FillStopReq(req_info, stop_info);            
        }       

        if (req.AsMap().at("type"s).AsString() == "Bus"s) {             
            std::optional<BusInfo> bus_info = catalogue.GetBusInfo(req.AsMap().at("name"s).AsString());
            //заполняет словарь req_info информацией об автобусе
            FillBusReq(req_info, bus_info);           
        }
        
        if (req.AsMap().at("type"s).AsString() == "Map"s) {             
            //добавляет карту в словарь req_info 
            req_info["map"] = Node(out.str());           
        }

        req_info["request_id"s] = Node(req.AsMap().at("id"s).AsInt());
        info.push_back(req_info);       
    }
    
    return Document(Node(info));
} 

void JSONReader::PrintInfo(const Document& doc, std::ostream& out) {    
    Print(doc, out);
}

std::vector<std::string> JSONReader::ProcessRoute(json::Node req) {
    std::vector<std::string> stops;
    for (auto stop : req.AsMap().at("stops"s).AsArray()) {               
        stops.push_back(stop.AsString());
    }

    if (req.AsMap().at("is_roundtrip"s).AsBool() == false) {
        for (int i = req.AsMap().at("stops"s).AsArray().size() - 2; i >= 0; --i) {
            stops.push_back(req.AsMap().at("stops"s).AsArray()[i].AsString());
        }         
    }              
    return stops;
}

void JSONReader::FillStopReq(Dict& req_info, const std::optional<StopInfo>& stop_info) {
    if (!stop_info) {
        req_info["error_message"s] = Node("not found"s);
        return void();
    }
    Array buses;               
    for (const auto& bus : stop_info.value().buses) {
        buses.push_back(Node(std::string(bus)));
    }            
    req_info["buses"s] = Node(buses);
}

void JSONReader::FillBusReq(Dict& req_info, const std::optional<BusInfo>& bus_info) {
    if (!bus_info) {
        req_info["error_message"s] = Node("not found"s);
        return void();
    }
    req_info["curvature"] = Node(bus_info.value().curvature);
    req_info["route_length"] = Node(bus_info.value().route_length);
    req_info["stop_count"] = Node(bus_info.value().stops_on_route);
    req_info["unique_stop_count"] = Node((int)bus_info.value().unique_stops_num);       
}

renderer::RenderSettings JSONReader::GetRenderSettings() {
    renderer::RenderSettings settings;
    settings.width = render_settings_.AsMap().at("width"s).AsDouble();
    settings.height = render_settings_.AsMap().at("height"s).AsDouble();
    settings.padding = render_settings_.AsMap().at("padding"s).AsDouble();
    settings.line_width = render_settings_.AsMap().at("line_width"s).AsDouble();
    settings.stop_radius = render_settings_.AsMap().at("stop_radius"s).AsDouble();
    settings.bus_label_font_size = render_settings_.AsMap().at("bus_label_font_size"s).AsInt();
    settings.stop_label_font_size = render_settings_.AsMap().at("stop_label_font_size"s).AsInt();
    settings.bus_label_offset = {render_settings_.AsMap().at("bus_label_offset"s).AsArray()[0].AsDouble(),
                                 render_settings_.AsMap().at("bus_label_offset"s).AsArray()[1].AsDouble()};
    settings.stop_label_offset = {render_settings_.AsMap().at("stop_label_offset"s).AsArray()[0].AsDouble(),
                                  render_settings_.AsMap().at("stop_label_offset"s).AsArray()[1].AsDouble()};
    settings.underlayer_width = render_settings_.AsMap().at("underlayer_width"s).AsDouble();
    settings.underlayer_color = ProcessColorNode(render_settings_.AsMap().at("underlayer_color"s));        
    settings.color_palette = ProcessPaletteNode(render_settings_.AsMap().at("color_palette"s));       

    return settings;
}

svg::Color JSONReader::ProcessColorNode(Node node) {        
    if (node.IsArray()) {
        if (node.AsArray().size() == 3) {
            return svg::Rgb({(uint8_t)node.AsArray()[0].AsInt(),
                             (uint8_t)node.AsArray()[1].AsInt(),
                             (uint8_t)node.AsArray()[2].AsInt()});
        } else {
            return svg::Rgba({(uint8_t)node.AsArray()[0].AsInt(),
                              (uint8_t)node.AsArray()[1].AsInt(),
                              (uint8_t)node.AsArray()[2].AsInt(),
                              node.AsArray()[3].AsDouble()});
        }
    } else {
        return node.AsString();
    }
}

std::vector<svg::Color> JSONReader::ProcessPaletteNode(Node node) {
    std::vector<svg::Color> colors;
    for (Node color : node.AsArray()) {
        colors.push_back(ProcessColorNode(color));
    }
    return colors;
}