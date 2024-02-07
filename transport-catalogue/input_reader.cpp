#include "input_reader.h"

#include <algorithm>
#include <cassert>
#include <iterator>
#include <iostream>
#include <string>
#include <utility>    

/**
 * Парсит строку вида "10.123,  -30.1837" и возвращает пару координат (широта, долгота)
 */
namespace transport_catalogue{
namespace input_processing{    

Coordinates ParseCoordinates(std::string_view str) {
    static const double nan = std::nan("");

    auto not_space = str.find_first_not_of(' ');
    auto comma = str.find(',');

    if (comma == str.npos) {
        return {nan, nan};
    }

    auto not_space2 = str.find_first_not_of(' ', comma + 1);

    double lat = std::stod(std::string(str.substr(not_space, comma - not_space)));
    double lng = std::stod(std::string(str.substr(not_space2)));

    return {lat, lng};
}
    
std::vector<std::pair<std::string, int>> ParseDistance (std::string_view str) {
    auto comma = str.find(',');
    comma = str.find(',', comma+1);
    std::vector<std::pair<std::string, int>> distances;
    while (comma != std::string::npos) {
        
        auto num_begin = str.find_first_not_of(' ', comma+1);
        auto num_end = str.find('m', num_begin);
        std::string number = (std::string)str.substr(num_begin, num_end - num_begin);
        int distance = std::stoi(number);

        auto stop_begin = str.find("t", num_end+1) + 2;
        stop_begin = str.find_first_not_of(' ', stop_begin);
        comma = str.find(',', comma+1);
        int stop_end {};
        if (comma == std::string::npos) {
            stop_end = str.find_last_not_of(' ') + 1;
        } else {
            stop_end = str.find_last_not_of(' ', comma);
        }
    
        std::string stop = (std::string)str.substr(stop_begin, stop_end - stop_begin);          
        distances.push_back({std::move(stop), distance});       
    }
    
    return distances;
}

/**
 * Удаляет пробелы в начале и конце строки
 */
std::string_view Trim(std::string_view string) {
    const auto start = string.find_first_not_of(' ');
    if (start == string.npos) {
        return {};
    }
    return string.substr(start, string.find_last_not_of(' ') + 1 - start);
}

/**
 * Разбивает строку string на n строк, с помощью указанного символа-разделителя delim
 */
std::vector<std::string_view> Split(std::string_view string, char delim) {
    std::vector<std::string_view> result;

    size_t pos = 0;
    while ((pos = string.find_first_not_of(' ', pos)) < string.length()) {
        auto delim_pos = string.find(delim, pos);
        if (delim_pos == string.npos) {
            delim_pos = string.size();
        }
        if (auto substr = Trim(string.substr(pos, delim_pos - pos)); !substr.empty()) {
            result.push_back(substr);
        }
        pos = delim_pos + 1;
    }

    return result;
}


 //Парсит маршрут.
 //Для кольцевого маршрута (A>B>C>A) возвращает массив названий остановок [A,B,C,A]
 //Для некольцевого маршрута (A-B-C-D) возвращает массив названий остановок [A,B,C,D,C,B,A]
 
std::vector<std::string_view> ParseRoute(std::string_view route) {
    if (route.find('>') != route.npos) {
        return Split(route, '>');
    }

    auto stops = Split(route, '-');
    std::vector<std::string_view> results(stops.begin(), stops.end());
    results.insert(results.end(), std::next(stops.rbegin()), stops.rend());

    return results;
}
    
CommandDescription ParseCommandDescription(std::string_view line) {
    auto colon_pos = line.find(':');
    if (colon_pos == line.npos) {
        return {};
    }

    auto space_pos = line.find(' ');
    if (space_pos >= colon_pos) {
        return {};
    }

    auto not_space = line.find_first_not_of(' ', space_pos);
    if (not_space >= colon_pos) {
        return {};
    }

    return {std::string(line.substr(0, space_pos)),
            std::string(line.substr(not_space, colon_pos - not_space)),
            std::string(line.substr(colon_pos + 1))};
}

void InputReader::ParseLine(std::string_view line) {
    auto command_description = ParseCommandDescription(line);
    if (command_description) {
        commands_.push_back(std::move(command_description));
    }
}

void InputReader::ApplyCommands([[maybe_unused]] TransportCatalogue& catalogue) const {
    // Реализуйте метод самостоятельно
    for (auto cmnd : commands_) {
        if (cmnd.command == "Stop") {
            catalogue.AddStop(cmnd.id, ParseCoordinates(cmnd.description));
        }
    }
    
    for (auto cmnd : commands_) {
        if (cmnd.command == "Bus") {
            std::vector<TransportCatalogue::Stop*> stops_to_add;
            std::vector<std::string_view> stops = ParseRoute(cmnd.description);
            for (auto stop : stops) {
                stops_to_add.push_back(catalogue.FindStop(stop));
            }
            catalogue.AddBus(cmnd.id, stops_to_add);
        }
    }
    
    for (auto cmnd : commands_) {
        if (cmnd.command == "Stop") {
            std::vector<std::pair<std::string, int>> parsed_distances = ParseDistance(cmnd.description);
            for (auto [stop_to, distance] : parsed_distances) {
                catalogue.SetDistance(catalogue.FindStop(cmnd.id), catalogue.FindStop(stop_to), distance);
            }            
        }
    }

}
}
}
