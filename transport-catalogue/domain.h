#pragma once

#include <algorithm>
#include <string>
#include <vector>

#include "geo.h"

//классы основных сущностей, описывают автобусы и остановки;

namespace domain {

    struct Bus;

    struct Stop {
        std::string name;
        geo::Coordinates coordinates;

        std::vector<Bus*> buses;
    };

    struct Bus {
        std::string name;
        std::vector<Stop*> stops_on_route;

        size_t count_stops_on_route;
        size_t count_unique_stops;
        double route_length;

        bool is_roundtrip;

    };

    struct BusStat {
        std::string_view name;
        bool not_found;
        int stops_on_route;
        int unique_stops;
        int route_length;
        double curvature;
    };

    struct StopStat {
        std::string_view name;
        bool not_found;
        std::vector <std::string> buses_name;
    };

    struct QueryStat {
        int id;
        std::string type;
        std::string name;
    };

} // end namespace domain
