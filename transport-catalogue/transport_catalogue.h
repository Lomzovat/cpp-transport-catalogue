#pragma once
#include <deque>
#include <string>
#include <vector>
#include <set>

#include <iomanip>
#include <iostream>

#include <unordered_set>
#include <unordered_map>

#include "domain.h"

using namespace domain;

namespace transport_catalogue {



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

        void AddDistance(const DistanceToStop& distances);

        double GetLength(Bus* bus);

        Bus* GetBus(std::string_view _bus_name);
        Stop* GetStop(std::string_view _stop_name);

        std::unordered_map<std::string_view, Bus*> GetBusnameToBus() const;
        std::unordered_map<std::string_view, Stop*> GetStopnameToStop() const;
        std::unordered_set<const Stop*> GetUniqStops(Bus* bus);

        std::unordered_set<const Bus*> StopGetUniqueBuses(Stop* stop);


        size_t GetDistanceStop(const Stop* start, const Stop* finish);
        size_t GetDistanceForBus(Bus* _bus);
    private:
        std::deque<Stop> stops;
        std::deque<Bus> buses;

        std::unordered_map<std::string_view, Stop*> stopname_to_stop;
        std::unordered_map<std::string_view, Bus*> busname_to_bus;

        DistanceToStop distance_to_stop;

    };
}