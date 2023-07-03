#pragma once
#include <deque>
#include <string>
#include <vector>
#include <set>

#include <iomanip>
#include <iostream>

#include <unordered_set>
#include <unordered_map>

#include "geo.h"

namespace transport_catalogue {

    struct Bus;

    struct Stop {
        std::string stop_name;
        geo::Coordinates coordinates;

        std::vector<Bus*> buses;
    };

    struct Bus {
        std::string bus_name;
        std::vector<Stop*> stops_on_route;

        size_t count_stops_on_route;
        size_t count_unique_stops;
        double route_length;


    };



    class DistanceHasher {
    public:
        std::size_t operator()(const std::pair<const Stop*, const Stop*> pair_stops) const noexcept {
            auto hash_1 = static_cast<const void*>(pair_stops.first);
            auto hash_2 = static_cast<const void*>(pair_stops.second);
            return hasher_(hash_1) * 17 + hasher_(hash_2);
        }
    private:
        std::hash<const void*> hasher_;
    };


    using DistanceToStop = std::unordered_map<std::pair<const Stop*, const Stop*>, int, DistanceHasher>;


    class TransportCatalogue {
    public:
        void AddBus(const Bus& bus);
        void AddStop(const Stop& stop);
        void AddDistance(const DistanceToStop distances);

        double GetLength(const Bus* bus);

        Bus* GetBus(std::string_view _bus_name);
        Stop* GetStop(std::string_view _stop_name);

        std::unordered_set<const Bus*> StopGetUniqueBuses(Stop* stop);

        size_t GetDistanceStop(const Stop* _start, const Stop* _finish);
        size_t GetDistanceForBus(Bus* _bus);
    private:
        std::deque<Stop> stops;
        std::deque<Bus> buses;

        std::unordered_map<std::string_view, Stop*> stopname_to_stop;
        std::unordered_map<std::string_view, Bus*> busname_to_bus;


        DistanceToStop distance_to_stop;

    };
}