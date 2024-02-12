#include "request_handler.h"

std::optional<BusInfo> RequestHandler::GetBusStat(const std::string_view& bus_name) const {
    return db_.GetBusInfo(std::string(bus_name));
}

std::set<std::string_view> RequestHandler::GetBusesByStop(const std::string_view& stop_name) const {
    return db_.GetStopInfo(std::string(stop_name)).value().buses;
}

std::set<std::string_view> RequestHandler::GetAllRoutes() {
    return db_.GetAllBuses();
}

std::vector<Bus*> RequestHandler::GetAllRoutesWithInfo() {
    std::vector<Bus*> buses;
    for (auto bus : GetAllRoutes()) {
        std::optional<BusInfo> bus_info = GetBusStat(bus);
        if (bus_info) {
            if (bus_info.value().stops_on_route == 0) {
                continue;
            } else {
                buses.push_back(db_.FindBus(bus));
            }
        } 
    }
    return buses;
}

void RequestHandler::RenderMap(std::ostringstream& out) const {
    renderer_.RenderMap(out);
}
