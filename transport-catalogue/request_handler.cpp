#include "request_handler.h"

using namespace std::literals;

namespace request_handler {

    class EdgeInfo {
    public:
        Node operator()(const StopItems& stop_info) {


            return Builder{}.StartDict()
                .Key("type").Value("Wait")
                .Key("stop_name").Value(std::string(stop_info.name))
                .Key("time").Value(stop_info.time)
                .EndDict()
                .Build();
        }

        Node operator()(const BusItems& bus_info) {


            return Builder{}.StartDict()
                .Key("type").Value("Bus")
                .Key("bus").Value(std::string(bus_info.bus_name))
                .Key("span_count").Value(static_cast<int>(bus_info.span_count))
                .Key("time").Value(bus_info.time)
                .EndDict()
                .Build();
        }
    };

    Node RequestHandler::GetNodeStop(int id_request, const StopStat& stop_info) {
        Node result;
        Array buses;
        Builder builder;

        std::string not_found = "not found";

        if (stop_info.not_found) {
            builder.StartDict()
                .Key("request_id").Value(id_request)
                .Key("error_message").Value(not_found)
                .EndDict();

            result = builder.Build();

        }
        else {
            builder.StartDict()
                .Key("request_id").Value(id_request)
                .Key("buses").StartArray();

            for (std::string bus_name : stop_info.buses_name) {
                builder.Value(bus_name);
            }

            builder.EndArray().EndDict();

            result = builder.Build();
        }

        return result;
    }

    Node RequestHandler::GetNodeBus(int id_request, const BusStat& bus_info) {
        Node result;
        std::string not_found = "not found";

        if (bus_info.not_found) {
            result = Builder{}.StartDict()
                .Key("request_id").Value(id_request)
                .Key("error_message").Value(not_found)
                .EndDict()
                .Build();
        }
        else {
            result = Builder{}.StartDict()
                .Key("request_id").Value(id_request)
                .Key("curvature").Value(bus_info.curvature)
                .Key("route_length").Value(bus_info.route_length)
                .Key("stop_count").Value(bus_info.stops_on_route)
                .Key("unique_stop_count").Value(bus_info.unique_stops)
                .EndDict()
                .Build();
        }

        return result;
    }

    Node RequestHandler::GetNodeMap(int id_request,
        TransportCatalogue& catalogue_,
        RenderSettings render_settings) {
        Node result;

        std::ostringstream map_stream;
        std::string map_str;

        MapRenderer map_catalogue(render_settings);
        map_catalogue.RenderSphereProjector(GetStopCoordinates(catalogue_));
        map_catalogue.ParseMapRender(map_catalogue, catalogue_);
        map_catalogue.GetMapStream(map_stream);
        map_str = map_stream.str();

        result = Builder{}.StartDict()
            .Key("request_id").Value(id_request)
            .Key("map").Value(map_str)
            .EndDict()
            .Build();

        return result;
    }

    Node RequestHandler::GetNodeRoute(QueryStat& request,
        TransportCatalogue& catalogue,
        TransportRouter& routing) {
        const auto& route_info = GetRouterInfo(request.from, request.to, catalogue, routing);

        if (!route_info) {
            return Builder{}.StartDict()
                .Key("request_id").Value(request.id)
                .Key("error_message").Value("not found")
                .EndDict()
                .Build();
        }

        Array items;
        for (const auto& item : route_info->edges) {
            items.emplace_back(std::visit(EdgeInfo{}, item));
        }

        return Builder{}.StartDict()
            .Key("request_id").Value(request.id)
            .Key("total_time").Value(route_info->total_time)
            .Key("items").Value(items)
            .EndDict()
            .Build();
    }

    void RequestHandler::ParseQuery(TransportCatalogue& catalogue,
        std::vector<QueryStat>& stat_requests,
        RenderSettings& render_settings,
        RoutingSettings& routing_settings) {

        std::vector<Node> result_request;
        TransportRouter routing;

        routing.SetRoutingSettings(routing_settings);
        routing.BuildRouter(catalogue);

        for (QueryStat req : stat_requests) {

            if (req.type == "Stop") {
                result_request.push_back(GetNodeStop(req.id, GetStopStat(catalogue, req.name)));

            }
            else if (req.type == "Bus") {
                result_request.push_back(GetNodeBus(req.id, GetBusStat(catalogue, req.name)));

            }
            else if (req.type == "Map") {
                result_request.push_back(GetNodeMap(req.id, catalogue, render_settings));

            }
            else if (req.type == "Route") {
                result_request.push_back(GetNodeRoute(req, catalogue, routing));
            }
        }

        document_out = Document{ Node(result_request) };
    }


    std::optional<RouteInfo> RequestHandler::GetRouterInfo(std::string_view start,
        std::string_view end,
        TransportCatalogue& catalogue,
        TransportRouter& routing) const {

        return routing.GetRouteInfo(routing.GetRouterByStop(catalogue.GetStop(start))->bus_wait_start,
            routing.GetRouterByStop(catalogue.GetStop(end))->bus_wait_start);
    }

    std::vector<geo::Coordinates> RequestHandler::GetStopCoordinates(TransportCatalogue& catalogue_) const {

        std::vector<geo::Coordinates> stops_coordinates;
        auto buses = catalogue_.GetBusnameToBus();

        for (auto& [busname, bus] : buses) {

            for (auto& stop : bus->stops_on_route) {
                geo::Coordinates coordinates;
                coordinates.latitude = stop->coordinates.latitude;
                coordinates.longitude = stop->coordinates.longitude;

                stops_coordinates.push_back(coordinates);
            }
        }
        return stops_coordinates;
    }


    BusStat RequestHandler::GetBusStat(TransportCatalogue& catalogue, std::string_view bus_name) {
        BusStat bus_info;
        Bus* bus = catalogue.GetBus(bus_name);

        if (bus != nullptr) {
            bus_info.name = bus->name;
            bus_info.not_found = false;
            bus_info.stops_on_route = bus->stops_on_route.size();
            bus_info.unique_stops = catalogue.GetUniqStops(bus).size();
            bus_info.route_length = bus->route_length;
            bus_info.curvature = double(catalogue.GetDistanceForBus(bus)
                / catalogue.GetLength(bus));
        }
        else {
            bus_info.name = bus_name;
            bus_info.not_found = true;
        }

        return bus_info;
    }

    StopStat RequestHandler::GetStopStat(TransportCatalogue& catalogue, std::string_view stop_name) {
        std::unordered_set<const Bus*> unique_buses;
        StopStat stop_info;
        Stop* stop = catalogue.GetStop(stop_name);

        if (stop != nullptr) {

            stop_info.name = stop->name;
            stop_info.not_found = false;
            unique_buses = catalogue.StopGetUniqBuses(stop);

            if (unique_buses.size() > 0) {

                for (const Bus* bus : unique_buses) {
                    stop_info.buses_name.push_back(bus->name);
                }

                std::sort(stop_info.buses_name.begin(), stop_info.buses_name.end());
            }

        }
        else {
            stop_info.name = stop_name;
            stop_info.not_found = true;
        }

        return stop_info;
    }

    const Document& RequestHandler::GetDocument() const {
        return document_out;
    }

}//end namespace request_handler