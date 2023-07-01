#include "transport_catalogue.h"

#include <execution>

namespace transport_catalogue {

    void TransportCatalogue::AddStop(const Stop& stop_) {
        stops.push_back(std::move(stop_));
        stopname_to_stop[stops.back().stop_name] = &stops.back();
    }

    void TransportCatalogue::AddBus(const Bus& bus_) {
        Bus* bus_buf;
        buses.push_back(std::move(bus_));
        bus_buf = &buses.back();
        busname_to_bus[buses.back().bus_name] = bus_buf;

        for (Stop* _stop : bus_buf->stops_on_route) {
            _stop->buses.push_back(bus_buf);
        }
        bus_buf->count_stops_on_route = bus_buf->stops_on_route.size();
        std::set<Stop*> unique_stops_on_route(bus_buf->stops_on_route.begin(),
            bus_buf->stops_on_route.end());
        bus_buf->count_unique_stops = unique_stops_on_route.size();
        bus_buf->route_length = GetDistanceForBus(bus_buf);
    }

    void TransportCatalogue::AddDistance(DistanceToStop distances) {
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
        if (stopname_to_stop.empty())
        {
            return nullptr;
        }
        if (stopname_to_stop.count(stop_name)) {
            return stopname_to_stop.at(stop_name);
        }
        else {
            return nullptr;
        }
    }

    double TransportCatalogue::GetLength(const Bus* bus) {
        return transform_reduce(next(bus->stops_on_route.begin()),
            bus->stops_on_route.end(),
            bus->stops_on_route.begin(),
            0.0,
            std::plus<>{},
            [](const Stop* lhs, const Stop* rhs) {
                return geo::ComputeDistance({ (*lhs).coordinates.latitude,
                                             (*lhs).coordinates.longitude }, 
                    { (*rhs).coordinates.latitude, (*rhs).coordinates.longitude });
            });
    }

    std::unordered_set<const Bus*> TransportCatalogue::StopGetUniqueBuses(Stop* stop) {
        std::unordered_set<const Bus*> unique_stops_;
        unique_stops_.insert(stop->buses.begin(), stop->buses.end());
        return unique_stops_;
    }

    size_t TransportCatalogue::GetDistanceStop(const Stop* _begin, const Stop* _finish) {
        if (distance_to_stop.empty()) {
            return 0;
        }

        auto dist_pair = std::make_pair(_begin, _finish);
        if (distance_to_stop.count(dist_pair)) {
            return distance_to_stop.at(dist_pair);
        }
        else {
            auto dist_pair = std::make_pair(_finish, _begin);
            return distance_to_stop.at(dist_pair);
        }
    }


    size_t TransportCatalogue::GetDistanceForBus(Bus* _bus) {
        size_t distance = 0;
        auto stops_size = _bus->stops_on_route.size() - 1;
        for (int i = 0; i < stops_size; i++) {
            distance += GetDistanceStop(_bus->stops_on_route[i], _bus->stops_on_route[i + 1]);
        }
        return distance;
    }
}