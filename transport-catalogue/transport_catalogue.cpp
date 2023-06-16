#include "transport_catalogue.h"

#include <execution>

namespace transport_catalogue {

    void TransportCatalogue::AddStop(Stop&& stop) {
        stops.push_back(std::move(stop));
        Stop* stop_buf = &stops.back();
        stopname_to_stop.insert(StopMap::value_type(stop_buf->name, stop_buf));
    }

    void TransportCatalogue::AddBus(Bus&& bus) {
        Bus* bus_buf;
        buses.push_back(std::move(bus));
        bus_buf = &buses.back();
        busname_to_bus.insert(BusMap::value_type(bus_buf->name, bus_buf));

        for (Stop* stop : bus_buf->stops) {
            stop->buses.push_back(bus_buf);
        }
    }

    void TransportCatalogue::AddDistance(std::vector<Distance> distances) {
        for (auto distance_ : distances) {
            auto dist_pair = std::make_pair(distance_.A, distance_.B);
            distance_to_stop.insert(DistanceMap::value_type(dist_pair, distance_.distance));
        }
    }

    Stop* TransportCatalogue::GetStop(std::string_view _stop_name) {
        if (stopname_to_stop.empty()) {
            return nullptr;
        }
        try {
            return stopname_to_stop.at(_stop_name);
        }
        catch (const std::out_of_range& e) {
            return nullptr;
        }
    }


    Bus* TransportCatalogue::GetBus(std::string_view _bus_name) {
        if (busname_to_bus.empty()) {
            return nullptr;
        }
        try {
            return busname_to_bus.at(_bus_name);
        }
        catch (const std::out_of_range& e) {
            return nullptr;
        }
    }


    std::unordered_set<const Stop*> TransportCatalogue::get_uniq_stops(Bus* bus) {
        std::unordered_set<const Stop*> unique_stops_;

        unique_stops_.insert(bus->stops.begin(), bus->stops.end());

        return unique_stops_;
    }

    double TransportCatalogue::get_length(Bus* bus) {
        return transform_reduce(next(bus->stops.begin()),
            bus->stops.end(),
            bus->stops.begin(),
            0.0,
            std::plus<>{},
            [](const Stop* lhs, const Stop* rhs) {
                return detail::geo::ComputeDistance({ (*lhs).latitude,
                                        (*lhs).longitude }, { (*rhs).latitude,
                                                              (*rhs).longitude });
            });
    }

    std::unordered_set<const Bus*> TransportCatalogue::stop_get_uniq_buses(Stop* stop) {
        std::unordered_set<const Bus*> unique_stops_;

        unique_stops_.insert(stop->buses.begin(), stop->buses.end());

        return unique_stops_;
    }

    size_t TransportCatalogue::get_distance_stop(const Stop* _begin, const Stop* _finish) {
        if (distance_to_stop.empty()) {
            return 0;
        }
        try {
            auto dist_pair = std::make_pair(_begin, _finish);
            return distance_to_stop.at(dist_pair);
        }
        catch (const std::out_of_range& e) {
            try {
                auto dist_pair = std::make_pair(_finish, _begin);
                return distance_to_stop.at(dist_pair);
            }
            catch (const std::out_of_range& e) {
                return 0;
            }
        }
    }

    size_t TransportCatalogue::get_distance_to_bus(Bus* _bus) {
        size_t distance = 0;
        auto stops_size = _bus->stops.size() - 1;
        for (int i = 0; i < stops_size; i++) {
            distance += get_distance_stop(_bus->stops[i], _bus->stops[i + 1]);
        }
        return distance;
    }
}