#include "request_handler.h"

namespace request_handler {



    Node RequestHandler::GetNodeStop(int id_request, StopStat stop_info) {
        Dict result;
        Array buses;
        std::string not_found = "not found";

        if (stop_info.not_found) {
            result.emplace("request_id", Node(id_request));
            result.emplace("error_message", Node(not_found));

        }
        else {
            result.emplace("request_id", Node(id_request));

            for (std::string bus_name : stop_info.buses_name) {
                buses.push_back(Node(bus_name));
            }

            result.emplace("buses", Node(buses));
        }

        return Node(result);
    }

    Node RequestHandler::GetNodeBus(int id_request, BusStat bus_info) {
        Dict result;
        std::string not_found = "not found";

        if (bus_info.not_found) {
            result.emplace("request_id", Node(id_request));
            result.emplace("error_message", Node(not_found));
        }
        else {
            result.emplace("request_id", Node(id_request));
            result.emplace("curvature", Node(bus_info.curvature));
            result.emplace("route_length", Node(bus_info.route_length));
            result.emplace("stop_count", Node(bus_info.stops_on_route));
            result.emplace("unique_stop_count", Node(bus_info.unique_stops));
        }

        return Node(result);
    }

    Node RequestHandler::GetNodeMap(int id_request, TransportCatalogue& catalogue, RenderSettings render_settings) {
        Dict result;
        std::ostringstream map_stream;
        std::string map_str;

        MapRenderer map_catalogue(render_settings);
        map_catalogue.RenderSphereProjector(GetStopCoordinates(catalogue));
        ParseMapRender(map_catalogue, catalogue);
        map_catalogue.GetMapStream(map_stream);
        map_str = map_stream.str();

        result.emplace("request_id", Node(id_request));
        result.emplace("map", Node(map_str));

        return Node(result);
    }

    void RequestHandler::ParseQuery(TransportCatalogue& catalogue, std::vector<QueryStat>& stat_requests, RenderSettings& render_settings) {
        std::vector<Node> result_request;

        for (const auto& request : stat_requests) {

            if (request.type == "Stop") {
                result_request.push_back(GetNodeStop(request.id, GetStopStat(catalogue, request.name)));
            }
            else if (request.type == "Bus") {
                result_request.push_back(GetNodeBus(request.id, GetBusStat(catalogue, request.name)));
            }
            else if (request.type == "Map") {
                result_request.push_back(GetNodeMap(request.id, catalogue, render_settings));
            }

        }

        document_out = Document{ Node(result_request) };
    }

    void RequestHandler::ParseMapRender(MapRenderer& map_catalogue, TransportCatalogue& catalogue) const {
        std::vector<std::pair<Bus*, int>> buses_palette;
        std::vector<Stop*> stops_sort;
        int palette_size = 0;
        int palette_index = 0;

        palette_size = map_catalogue.GetPaletteSize();
        if (palette_size == 0) {
            std::cout << "color palette is empty";
            return;
        }

        auto buses = catalogue.GetBusnameToBus();
        if (buses.size() > 0) {

            for (std::string_view bus_name : GetSortedBusesNames(catalogue)) {
                Bus* bus_info = catalogue.GetBus(bus_name);

                if (bus_info) {
                    if (bus_info->stops_on_route.size() > 0) {
                        buses_palette.push_back(std::make_pair(bus_info, palette_index));
                        palette_index++;

                        if (palette_index == palette_size) {
                            palette_index = 0;
                        }
                    }
                }
            }

            if (buses_palette.size() > 0) {
                map_catalogue.AddLine(buses_palette);
                map_catalogue.AddBusRoute(buses_palette);
            }
        }

        auto stops = catalogue.GetStopnameToStop();
        if (stops.size() > 0) {
            std::vector<std::string_view> stops_name;

            for (const auto& [stop_name, stop] : stops) {

                if (stop->buses.size() > 0) {
                    stops_name.push_back(stop_name);
                }
            }

            std::sort(stops_name.begin(), stops_name.end());

            for (std::string_view stop_name : stops_name) {
                Stop* stop = catalogue.GetStop(stop_name);
                if (stop) {
                    stops_sort.push_back(stop);
                }
            }

            if (stops_sort.size() > 0) {
                map_catalogue.AddStopCircle(stops_sort);
                map_catalogue.AddStopName(stops_sort);
            }
        }
    }

    std::vector<geo::Coordinates> RequestHandler::GetStopCoordinates(TransportCatalogue& catalog) const {

        std::vector<geo::Coordinates> stops_coordinates;
        auto buses = catalog.GetBusnameToBus();

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

    std::vector<std::string_view> RequestHandler::GetSortedBusesNames(TransportCatalogue& catalog) const {
        std::vector<std::string_view> buses_names;

        auto buses = catalog.GetBusnameToBus();
        if (buses.size() > 0) {

            for (auto& [busname, bus] : buses) {
                buses_names.push_back(busname);
            }

            std::sort(buses_names.begin(), buses_names.end());

            return buses_names;

        }
        else {
            return {};
        }
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
            unique_buses = catalogue.StopGetUniqueBuses(stop);

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
}