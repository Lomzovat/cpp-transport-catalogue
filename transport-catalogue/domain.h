#pragma once

#include <algorithm>
#include <vector>
#include <string>
#include <variant>

#include "geo.h"
#include "graph.h"

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
        std::string from;
        std::string to;
    };

    struct StopItems {
        std::string_view name;
        double time = 0.0;
    };

    struct BusItems {
        std::string_view bus_name;
        size_t span_count = 0.0;
        double time = 0.0;
    };

    struct RoutingSettings {
        double bus_wait_time = 0.0;
        double bus_velocity = 0.0;
    };

    struct RouterByStop {
        graph::VertexId bus_wait_start;
        graph::VertexId bus_wait_end;
    };

    struct RouteInfo {
        double total_time = 0.0;
        std::vector<std::variant<StopItems, BusItems>> edges;
    };

}//end namespace domain

