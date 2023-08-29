#include "transport_catalogue.h"
#include <execution>

namespace transport_catalogue {

    void TransportCatalogue::AddStop(const Stop& stop) {
        stops.push_back(std::move(stop));
        stopname_to_stop[stops.back().name] = &stops.back();
    }

    void TransportCatalogue::AddBus(const Bus& bus) {
        Bus* bus_buf;

        buses.push_back(std::move(bus));
        bus_buf = &buses.back();
        busname_to_bus[buses.back().name] = bus_buf;

        for (Stop* stop : bus_buf->stops_on_route) {
            stop->buses.push_back(bus_buf);
        }

        bus_buf->route_length = GetDistanceForBus(bus_buf);
    }

    void TransportCatalogue::AddDistance(const DistanceToStop& distances) {

        for (const auto& [key, value] : distances) {
            distance_to_stop[key] = value;
        }
    }

    Bus* TransportCatalogue::GetBus(std::string_view bus_name) {
        if (busname_to_bus.empty()) {
            return nullptr;
        }

        if (busname_to_bus.count(bus_name)) {
            return busname_to_bus.at(bus_name);
        }
        else {
            return nullptr;
        }
    }

    Stop* TransportCatalogue::GetStop(std::string_view stop_name) {
        if (stopname_to_stop.empty()) {
            return nullptr;
        }

        if (stopname_to_stop.count(stop_name)) {
            return stopname_to_stop.at(stop_name);
        }
        else {
            return nullptr;
        }
    }

    std::unordered_map<std::string_view, Bus*>  TransportCatalogue::GetBusnameToBus() const {
        return busname_to_bus;
    }

    std::unordered_map<std::string_view, Stop*> TransportCatalogue::GetStopnameToStop() const {
        return stopname_to_stop;
    }

    std::unordered_set<const Stop*> TransportCatalogue::GetUniqStops(Bus* bus) {
        std::unordered_set<const Stop*> unique_stops;
        unique_stops.insert(bus->stops_on_route.begin(), bus->stops_on_route.end());

        return unique_stops;
    }

    double TransportCatalogue::GetLength(Bus* bus) {
        return transform_reduce(next(bus->stops_on_route.begin()),
            bus->stops_on_route.end(),
            bus->stops_on_route.begin(),
            0.0,
            std::plus<>{},
            [](const Stop* lhs, const Stop* rhs) {
                return geo::ComputeDistance({ (*lhs).coordinates.latitude,
                                              (*lhs).coordinates.longitude }, { (*rhs).coordinates.latitude,
                                                                  (*rhs).coordinates.longitude });
            });
    }

    std::unordered_set<const Bus*> TransportCatalogue::StopGetUniqBuses(Stop* stop) {
        std::unordered_set<const Bus*> unique_stops;
        unique_stops.insert(stop->buses.begin(), stop->buses.end());

        return unique_stops;
    }

    size_t TransportCatalogue::GetDistanceStop(const Stop* begin, const Stop* finish) const {

        if (distance_to_stop.empty()) {
            return 0;
        }

        auto dist_pair = std::make_pair(begin, finish);
        if (distance_to_stop.count(dist_pair)) {
            return distance_to_stop.at(dist_pair);
        }
        else {
            auto dist_pair = std::make_pair(finish, begin);
            return distance_to_stop.at(dist_pair);
        }
    }

    size_t TransportCatalogue::GetDistanceForBus(Bus* bus) {
        size_t distance = 0;
        auto stops_size = bus->stops_on_route.size() - 1;

        for (int i = 0; i < static_cast<int>(stops_size); i++) {
            distance += GetDistanceStop(bus->stops_on_route[i], bus->stops_on_route[i + 1]);
        }

        return distance;
    }

}//end namespace transport_catalogue