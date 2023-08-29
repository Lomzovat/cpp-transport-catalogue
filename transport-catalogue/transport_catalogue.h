#pragma once
#include <deque>
#include <string>
#include <vector>
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
            return hasher(hash_1) * 17 + hasher(hash_2);
        }
    private:
        std::hash<const void*> hasher;
    };

    using DistanceToStop = std::unordered_map<std::pair<const Stop*, const Stop*>, int, DistanceHasher>;


    class TransportCatalogue {
    public:
        void AddBus(const Bus& bus);
        void AddStop(const Stop& stop);
        void AddDistance(const DistanceToStop& distances);

        Bus* GetBus(std::string_view bus_name);
        Stop* GetStop(std::string_view stop_name);
        std::unordered_map<std::string_view, Bus*>  GetBusnameToBus() const;
        std::unordered_map<std::string_view, Stop*> GetStopnameToStop() const;

        std::unordered_set<const Bus*> StopGetUniqBuses(Stop* stop);
        std::unordered_set<const Stop*> GetUniqStops(Bus* bus);
        double GetLength(Bus* bus);

        size_t GetDistanceStop(const Stop* start, const Stop* finish) const;
        size_t GetDistanceForBus(Bus* bus);

    private:
        std::deque<Stop> stops;
        std::deque<Bus> buses;

        std::unordered_map<std::string_view, Stop*> stopname_to_stop;
        std::unordered_map<std::string_view, Bus*> busname_to_bus;

        DistanceToStop distance_to_stop;

    };