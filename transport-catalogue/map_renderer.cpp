#include "map_renderer.h"

namespace renderer {
    
    svg::Point SphereProjector::operator()(geo::Coordinates coords) const {
        return {
            (coords.lng - min_lon_) * zoom_coeff_ + padding_,
            (max_lat_ - coords.lat) * zoom_coeff_ + padding_
        };
    }
    
    void RoutesRenderer::Draw(svg::ObjectContainer& document) const {    
        SphereProjector projector = CreateProjector();
        AddRouteLines(document, projector);
        AddRouteNames(document, projector);   
        std::set<Stop*, cmp> stops_to_render = GetStopsToRender();        
        AddStopCircles(stops_to_render, document, projector);
        AddStopNames(stops_to_render, document, projector);        
    }
    
    void RoutesRenderer::SetBusesToRender(const std::vector<Bus*>& buses) {
        buses_to_render_ = buses;
    }
    
    void RoutesRenderer::SetSettings(const RenderSettings& settings) {
        settings_ = settings;
    }
    
    void RoutesRenderer::SetInfoBusesToRoundtrip(std::map<std::string, bool> buses_to_roundtrip) {
        buses_to_roundtrip_ = buses_to_roundtrip;
    }

    void RoutesRenderer::FillWithCoordinates(std::vector<geo::Coordinates>& coordinates) const {
        for (Bus* bus : buses_to_render_) {
            for (Stop* stop : bus->stops_on_route) {
                coordinates.push_back(stop->coordinates);
            }
        }
    }
    
    SphereProjector RoutesRenderer::CreateProjector() const {
        std::vector<geo::Coordinates> coordinates; 
        FillWithCoordinates(coordinates);
        SphereProjector projector(coordinates.begin(), coordinates.end(), settings_.width, settings_.height, settings_.padding);
        return projector;
    }
    
    svg::Polyline RoutesRenderer::CreateRoute(Bus* bus, const SphereProjector& projector) const {
        svg::Polyline polyline;
        for (Stop* stop : bus->stops_on_route) {
            polyline.AddPoint(projector(stop->coordinates));
        }
        return polyline;
    }
    
    void RoutesRenderer::AddRouteLines(svg::ObjectContainer& document, const SphereProjector& projector) const {
        size_t color_index = 0;
        size_t palette_size = settings_.color_palette.size();
        
        for (Bus* bus : buses_to_render_) {
            svg::Polyline route = CreateRoute(bus, projector);
            route 
                 .SetStrokeColor(settings_.color_palette[color_index])
                 .SetFillColor(svg::NoneColor)
                 .SetStrokeWidth(settings_.line_width)
                 .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                 .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
                
            ++color_index;
            color_index %=  palette_size;
            
            document.Add(route);
        }
        color_index = 0;
    }
    
    void RoutesRenderer::AddBusNameUnderlayer(Bus* bus, svg::Point position, svg::ObjectContainer& document) const {
        document.Add(svg::Text()
                     .SetPosition(position)
                     .SetOffset(settings_.bus_label_offset)
                     .SetFontSize(settings_.bus_label_font_size)
                     .SetFontFamily("Verdana"s)
                     .SetFontWeight("bold"s)
                     .SetData(bus->route)
                     .SetFillColor(settings_.underlayer_color)
                     .SetStrokeColor(settings_.underlayer_color)
                     .SetStrokeWidth(settings_.underlayer_width)
                     .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                     .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND));
    }
    
    void RoutesRenderer::AddBusNameLabel(Bus* bus, svg::Color color, svg::Point position, svg::ObjectContainer& document) const {
        document.Add(svg::Text()
                     .SetPosition(position)
                     .SetOffset(settings_.bus_label_offset)
                     .SetFontSize(settings_.bus_label_font_size)
                     .SetFontFamily("Verdana"s)
                     .SetFontWeight("bold"s)
                     .SetData(bus->route)
                     .SetFillColor(color)); 
    }
    
    void RoutesRenderer::AddRouteNames(svg::ObjectContainer& document, const SphereProjector& projector) const {        
        size_t color_index = 0;
        size_t palette_size = settings_.color_palette.size();        
    
        for (Bus* bus : buses_to_render_) {
            AddBusNameUnderlayer(bus, projector(bus->stops_on_route[0]->coordinates), document);
            AddBusNameLabel(bus, settings_.color_palette[color_index], projector(bus->stops_on_route[0]->coordinates), document);                  
            
            if ((!buses_to_roundtrip_.at(bus->route)) &&
                (bus->stops_on_route[0]->name_of_stop != bus->stops_on_route[(bus->stops_on_route).size()/2]->name_of_stop)) {
                AddBusNameUnderlayer(bus, projector(bus->stops_on_route[(bus->stops_on_route).size()/2]->coordinates), document);
                AddBusNameLabel(bus, settings_.color_palette[color_index], projector(bus->stops_on_route[(bus->stops_on_route).size()/2]->coordinates), document);                       
            } 
            ++color_index;
            color_index %=  palette_size;
        }
    }
    
    std::set<Stop*, RoutesRenderer::cmp> RoutesRenderer::GetStopsToRender() const {        
        std::set<Stop*, cmp> stops_to_render;
        
        for(Bus* bus : buses_to_render_) {
            for (Stop* stop : bus->stops_on_route) {
                stops_to_render.insert(stop);
            }
        }
        return stops_to_render;
    }
    
    void RoutesRenderer::AddStopCircles(std::set<Stop*, cmp> stops_to_render, svg::ObjectContainer& document, const SphereProjector& projector) const {        
        for (Stop* stop : stops_to_render) {
            document.Add(svg::Circle()
                                    .SetCenter(projector(stop->coordinates))
                                    .SetRadius(settings_.stop_radius)
                                    .SetFillColor("white"s));
        }
    }
    
    void RoutesRenderer::AddStopNameUnderlayer(Stop* stop, svg::Point position, svg::ObjectContainer& document) const {
        document.Add(svg::Text()
                             .SetPosition(position)
                             .SetOffset(settings_.stop_label_offset)
                             .SetFontSize(settings_.stop_label_font_size)
                             .SetFontFamily("Verdana"s)                                  
                             .SetData(stop->name_of_stop)
                             .SetFillColor(settings_.underlayer_color)
                             .SetStrokeColor(settings_.underlayer_color)
                             .SetStrokeWidth(settings_.underlayer_width)
                             .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                             .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND));
    }
    
    void RoutesRenderer::AddStopNameLabel(Stop* stop, svg::Point position, svg::ObjectContainer& document) const {
        document.Add(svg::Text()
                             .SetPosition(position)
                             .SetOffset(settings_.stop_label_offset)
                             .SetFontSize(settings_.stop_label_font_size)
                             .SetFontFamily("Verdana"s)                                  
                             .SetData(stop->name_of_stop)
                             .SetFillColor("black"s));     
    }
    
    void RoutesRenderer::AddStopNames(std::set<Stop*, cmp> stops_to_render, svg::ObjectContainer& document, const SphereProjector& projector) const {
         for (Stop* stop : stops_to_render) {
            AddStopNameUnderlayer(stop, projector(stop->coordinates), document);
            AddStopNameLabel(stop, projector(stop->coordinates), document);    
        }
    } 
    
    void MapRenderer::SetBusesToRender(std::vector<Bus*> buses_to_render) {
        buses_to_render_ = buses_to_render;
    }
    
    void MapRenderer::SetInfoBusesToRoundtrip(std::map<std::string, bool> buses_to_roundtrip) {
        buses_to_roundtrip_ = buses_to_roundtrip;
    }
    
    void MapRenderer::RenderMap(std::ostream& out) const {
        RoutesRenderer routes_renderer;
        routes_renderer.SetSettings(settings_);
        routes_renderer.SetBusesToRender(buses_to_render_);
        routes_renderer.SetInfoBusesToRoundtrip(buses_to_roundtrip_);
        svg::Document doc;
        routes_renderer.Draw(doc);
        doc.Render(out);
    } 
    
} //namespace renderer