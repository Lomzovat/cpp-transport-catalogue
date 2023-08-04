#include "json_reader.h"


using namespace transport_catalogue;


namespace json {

    JsonReader::JsonReader(Document document) :
        document_(std::move(document))
    {}
    JsonReader::JsonReader(std::istream& input) :
        document_(Load(input))
    {}

    Stop JsonReader::ParseNodeStop(Node& node) {
        Stop stop;
        Dict stop_node;

        if (node.IsDict()) {
            stop_node = node.AsDict();
            stop.name = stop_node.at("name").AsString();
            stop.coordinates.latitude = stop_node.at("latitude").AsDouble();
            stop.coordinates.longitude = stop_node.at("longitude").AsDouble();
        }
        return stop;
    }

    DistanceToStop JsonReader::ParseNodeDistance(Node& node, TransportCatalogue& catalogue) {
        DistanceToStop distance_to_stop;
        Dict stop_node;
        Dict road_distances;
        std::string begin_name;

        if (node.IsDict()) {
            stop_node = node.AsDict();
            begin_name = stop_node.at("name").AsString();

            road_distances = stop_node.at("road_distances").AsDict();
            for (auto [dest, dist] : road_distances) {
                std::string last_name = dest;
                int distance = dist.AsInt();
                distance_to_stop[std::make_pair(catalogue.GetStop(begin_name),
                    catalogue.GetStop(last_name))] = distance;
            }
        }
        else {
            std::cout << "error" << std::endl;
        }
        return distance_to_stop;
    }

    Bus JsonReader::ParseNodeBus(Node& node, TransportCatalogue& catalogue) {
        Bus bus;
        Array bus_stops;
        Dict bus_node;

        if (node.IsDict()) {
            bus_node = node.AsDict();
            bus.name = bus_node.at("name").AsString();
            bus.is_roundtrip = bus_node.at("is_roundtrip").AsBool();
            bus_stops = bus_node.at("stops").AsArray();
            for (auto& stop : bus_stops) {
                bus.stops_on_route.push_back(catalogue.GetStop(stop.AsString()));
            }
            if (!bus.is_roundtrip) {
                size_t size = bus.stops_on_route.size() - 1;
                for (size_t i = size; i > 0; i--) {
                    bus.stops_on_route.push_back(bus.stops_on_route[i - 1]);
                }
            }
        }
        else {
            std::cout << "error" << std::endl;
        }
        return bus;
    }

    void JsonReader::ParseBaseRequest(const Node& root, TransportCatalogue& catalogue) {
        Array base_requests;
        Node req_node;
        Dict request_map;

        std::vector<Node> buses;
        std::vector<Node> stops;

        if (root.IsArray()) {
            base_requests = root.AsArray();
            for (auto& node : base_requests) {
                if (node.IsDict()) {
                    request_map = node.AsDict();
                    req_node = request_map.at("type");
                    if (req_node.IsString()) {
                        if (req_node.AsString() == "Bus") {
                            buses.push_back(request_map);
                        }
                        else if (req_node.AsString() == "Stop") {
                            stops.push_back(request_map);
                        }
                        else {
                            std::cout << "base_requests are invalid";
                        }
                    }
                }
                else {
                    std::cout << "error";
                }
            }

            for (auto& stop : stops) {
                catalogue.AddStop(ParseNodeStop(stop));
            }

            for (auto& stop : stops) {
                catalogue.AddDistance(ParseNodeDistance(stop, catalogue));
            }

            for (auto& bus : buses) {
                catalogue.AddBus(ParseNodeBus(bus, catalogue));
            }
        }
        else {
            std::cout << "error";
        }
    }

    void JsonReader::ParseStatRequest(const Node& node, std::vector<QueryStat>& stat_request) {
        Array stat_requests;
        QueryStat req;
        Dict request_map;

        if (node.IsArray()) {
            stat_requests = node.AsArray();

            for (auto& req_node : stat_requests) {

                if (req_node.IsDict()) {
                    request_map = req_node.AsDict();
                    req.id = request_map.at("id").AsInt();
                    req.type = request_map.at("type").AsString();

                    if (req.type != "Map") {
                        req.name = request_map.at("name").AsString();
                    }
                    else {
                        req.name = "";
                    }
                    stat_request.push_back(req);
                }
            }
        }
        else {
            std::cout << "error";
        }
    }



    void JsonReader::ParseRenderRequest(const Node& node, map_renderer::RenderSettings& render_settings) {
        Dict render_map;

        if (node.IsDict()) {
            render_map = node.AsDict();

            render_settings.width_ = render_map.at("width").AsDouble();
            render_settings.height_ = render_map.at("height").AsDouble();
            render_settings.padding_ = render_map.at("padding").AsDouble();
            render_settings.line_width_ = render_map.at("line_width").AsDouble();
            render_settings.stop_radius_ = render_map.at("stop_radius").AsDouble();
            render_settings.bus_label_font_size_ = render_map.at("bus_label_font_size").AsInt();

            if (render_map.at("bus_label_offset").IsArray()) {
                Array bus_lab_offset = render_map.at("bus_label_offset").AsArray();
                render_settings.bus_label_offset_ = std::make_pair(bus_lab_offset[0].AsDouble(),
                    bus_lab_offset[1].AsDouble());
            }

            render_settings.stop_label_font_size_ = render_map.at("stop_label_font_size").AsInt();

            if (render_map.at("stop_label_offset").IsArray()) {
                Array stop_lab_offset = render_map.at("stop_label_offset").AsArray();
                render_settings.stop_label_offset_ = std::make_pair(stop_lab_offset[0].AsDouble(),
                    stop_lab_offset[1].AsDouble());
            }

            if (render_map.at("underlayer_color").IsString()) {
                render_settings.underlayer_color_ = svg::Color(render_map.at("underlayer_color").AsString());
            }
            else if (render_map.at("underlayer_color").IsArray()) {
                Array arr_color = render_map.at("underlayer_color").AsArray();
                render_settings.underlayer_color_ = (WorkWithColor(arr_color));
            }

            render_settings.underlayer_width_ = render_map.at("underlayer_width").AsDouble();

            if (render_map.at("color_palette").IsArray()) {
                Array arr_palette = render_map.at("color_palette").AsArray();

                for (Node color_palette : arr_palette) {

                    if (color_palette.IsString()) {
                        render_settings.color_palette_.push_back(svg::Color(color_palette.AsString()));
                    }
                    else if (color_palette.IsArray()) {
                        Array arr_color = color_palette.AsArray();
                        render_settings.color_palette_.push_back(WorkWithColor(arr_color));

                    }
                }
            }
        }
        else {
            std::cout << "unable to set settings";
        }
    }


    svg::Color JsonReader::WorkWithColor(Array& arr_color) {
        svg::Color color_;
        uint8_t red_ = arr_color[0].AsInt();
        uint8_t green_ = arr_color[1].AsInt();
        uint8_t blue_ = arr_color[2].AsInt();

        if (arr_color.size() == 4) {
            double opacity_ = arr_color[3].AsDouble();
            color_ = svg::Color(svg::Rgba(red_, green_, blue_, opacity_));
        }
        else if (arr_color.size() == 3) {
            color_ = svg::Color(svg::Rgb(red_, green_, blue_));
        }
        return color_;
    }



    void JsonReader::ParseNode(const Node& root, TransportCatalogue& catalogue, std::vector<QueryStat>& stat_request, map_renderer::RenderSettings& render_settings) {
        Dict root_dictionary;

        if (root.IsDict()) {
            root_dictionary = root.AsDict();

            ParseBaseRequest(root_dictionary.at("base_requests"), catalogue);
            ParseStatRequest(root_dictionary.at("stat_requests"), stat_request);
            ParseRenderRequest(root_dictionary.at("render_settings"), render_settings);

        }
        else {
            std::cout << "error";
        }
    }

    void JsonReader::Parse(TransportCatalogue& catalogue, std::vector<QueryStat>& stat_request, map_renderer::RenderSettings& render_settings) {
        ParseNode(document_.GetRoot(),
            catalogue,
            stat_request,
            render_settings);
    }
} // end namespace json