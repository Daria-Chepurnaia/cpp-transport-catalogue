#pragma once

#include "geo.h"
#include "svg.h"
#include "domain.h"

#include <algorithm>
#include <cstdlib>
#include <optional>
#include <vector>
#include <variant>
#include <map>
#include <set>

using namespace std::literals;

namespace renderer {
    
inline const double EPSILON = 1e-6;

class SphereProjector {
public:
    // points_begin and points_end define the beginning and end of the range of geo::Coordinates elements
    template <typename PointInputIt>
    SphereProjector(PointInputIt points_begin, PointInputIt points_end,
                    double max_width, double max_height, double padding);
    
    // Projects latitude and longitude into coordinates within an SVG image
    svg::Point operator()(geo::Coordinates coords) const;     

    bool IsZero(double value) {
        return std::abs(value) < EPSILON;
    }

private:
    double padding_;
    double min_lon_ = 0;
    double max_lat_ = 0;
    double zoom_coeff_ = 0;
};

struct RenderSettings {
    double width{};
    double height{};
    double padding{};
    double line_width{};
    double stop_radius{};
    int bus_label_font_size{};
    int stop_label_font_size{};
    svg::Point bus_label_offset;
    svg::Point stop_label_offset;
    svg::Color underlayer_color;
    double underlayer_width{};
    std::vector<svg::Color> color_palette;    
};

class RoutesRenderer : public svg::Drawable {
public:
    void Draw(svg::ObjectContainer& document) const override;

    void SetBusesToRender(const std::vector<Bus*>& buses);    
    void SetSettings(const RenderSettings& settings);    
    void SetInfoBusesToRoundtrip(std::map<std::string, bool> buses_to_roundtrip);  

private:
    std::vector<Bus*> buses_to_render_;
    RenderSettings settings_;
    std::map<std::string, bool> buses_to_roundtrip_;
    
    struct cmp {
        bool operator() (const Stop* left, const Stop* right) const {
            return left->name_of_stop < right->name_of_stop;
        }
    };

    void FillWithCoordinates(std::vector<geo::Coordinates>& coordinates) const;

    SphereProjector CreateProjector() const; 

    svg::Polyline CreateRoute(Bus* bus, const SphereProjector& projector) const;    
    void AddRouteLines(svg::ObjectContainer& document, const SphereProjector& projector) const;

    void AddBusNameUnderlayer(Bus* bus, svg::Point position, svg::ObjectContainer& document) const;    
    void AddBusNameLabel(Bus* bus, svg::Color color, svg::Point position, svg::ObjectContainer& document) const;    
    void AddRouteNames(svg::ObjectContainer& document, const SphereProjector& projector) const; 

    std::set<Stop*, cmp> GetStopsToRender() const;    
    void AddStopCircles(std::set<Stop*, cmp> stops_to_render, svg::ObjectContainer& document, const SphereProjector& projector) const;    
    void AddStopNameUnderlayer(Stop* stop, svg::Point position, svg::ObjectContainer& document) const;    
    void AddStopNameLabel(Stop* stop, svg::Point position, svg::ObjectContainer& document) const;    
    void AddStopNames(std::set<Stop*, cmp> stops_to_render, svg::ObjectContainer& document, const SphereProjector& projector) const;
};

class MapRenderer {
public:
    MapRenderer(RenderSettings settings)
        : settings_(settings)        
        {            
        }
    
    void SetBusesToRender(std::vector<Bus*> buses_to_render);
    void SetInfoBusesToRoundtrip(std::map<std::string, bool> buses_to_roundtrip);
    
    void RenderMap(std::ostream& out) const;
    
private:    
    RenderSettings settings_;
    std::vector<Bus*> buses_to_render_;
    std::map<std::string, bool> buses_to_roundtrip_;    
};

template <typename PointInputIt>
SphereProjector::SphereProjector(PointInputIt points_begin, PointInputIt points_end,
                                  double max_width, double max_height, double padding)
    : padding_(padding) 
{
    // If sphere surface points are not specified, there is nothing to compute
    if (points_begin == points_end) {
        return;
    }

    // Find points with minimum and maximum longitude
    const auto [left_it, right_it] = std::minmax_element(
        points_begin, points_end,
        [](auto lhs, auto rhs) { return lhs.lng < rhs.lng; });
    min_lon_ = left_it->lng;
    const double max_lon = right_it->lng;

    // Find points with minimum and maximum latitude
    const auto [bottom_it, top_it] = std::minmax_element(
        points_begin, points_end,
        [](auto lhs, auto rhs) { return lhs.lat < rhs.lat; });
    const double min_lat = bottom_it->lat;
    max_lat_ = top_it->lat;

    // Calculate the scaling factor along the x coordinate
    std::optional<double> width_zoom;
    if (!IsZero(max_lon - min_lon_)) {
        width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
    }

    // Calculate the scaling factor along the y coordinate
    std::optional<double> height_zoom;
    if (!IsZero(max_lat_ - min_lat)) {
        height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
    }

    if (width_zoom && height_zoom) {
        // Both scaling factors are non-zero, take the minimum of them
        zoom_coeff_ = std::min(*width_zoom, *height_zoom);
    } else if (width_zoom) {
        // Width scaling factor is non-zero, use it
        zoom_coeff_ = *width_zoom;
    } else if (height_zoom) {
        // Height scaling factor is non-zero, use it
        zoom_coeff_ = *height_zoom;
    }
}
    
} // namespace renderer
