#pragma once
#include <deque>
#include <string>
#include <vector>

#include <iomanip>
#include <iostream>

#include <unordered_set>
#include <unordered_map>

#include "geo.h"

namespace transport_catalogue {

    struct Bus;

    struct Stop {
        std::string name;
        double latitude;
        double longitude;

        std::vector<Bus*> buses;
    };

    struct Bus {
        std::string name;
        std::vector<Stop*> stops;
    };

    struct Distance {
        const Stop* A;
        const Stop* B;
        int distance;
    };

    class DistanceHasher {
    public:
        std::size_t operator()(const std::pair<const Stop*, const Stop*> pair_stops) const noexcept {
            auto hash1 = static_cast<const void*>(pair_stops.first);
            auto hash2 = static_cast<const void*>(pair_stops.second);
            return hasher_(hash1) * 17 + hasher_(hash2);
        }
    private:
        std::hash<const void*> hasher_;//struct
    };

    typedef  std::unordered_map<std::string_view, Stop*> StopMap;
    typedef  std::unordered_map<std::string_view, Bus*> BusMap;
    typedef  std::unordered_map<std::pair<const Stop*, const Stop*>, int, DistanceHasher> DistanceMap;

    class TransportCatalogue {
    public:
        void AddStop(Stop&& stop);
        void AddBus(Bus&& bus);
        void AddDistance(std::vector<Distance> distances);

        Stop* GetStop(std::string_view _stop_name);
        Bus* GetBus(std::string_view _bus_name);


        std::unordered_set<const Bus*> stop_get_uniq_buses(Stop* stop);
        std::unordered_set<const Stop*> get_uniq_stops(Bus* bus);
        double get_length(Bus* bus);

        size_t get_distance_stop(const Stop* _start, const Stop* _finish);
        size_t get_distance_to_bus(Bus* _bus);
    private:
        std::deque<Stop> stops;
        StopMap stopname_to_stop;

        std::deque<Bus> buses;
        BusMap busname_to_bus;

        DistanceMap distance_to_stop;
    };

}