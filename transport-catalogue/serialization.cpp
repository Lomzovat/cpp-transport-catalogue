#include "serialization.h"

namespace serialization {
    template <typename It>
    uint32_t FindStopId(It start, It end, std::string_view name) {

        auto stop_it = std::find_if(start, end, [&name](const domain::Stop stop) {return stop.name == name; });
        return std::distance(start, stop_it);
    }

    transport_catalogue_protobuf::Stop SerializeStops(const Stop& stop) {
        transport_catalogue_protobuf::Stop stop_proto;

        stop_proto.set_id(stop.id);
        stop_proto.set_name(stop.name);
        stop_proto.mutable_coordinates()->set_latitude(stop.coordinates.latitude);
        stop_proto.mutable_coordinates()->set_longitude(stop.coordinates.longitude);
        return stop_proto;
    }

    transport_catalogue_protobuf::Bus SerializeBuses(const Bus& bus, const transport_catalogue::TransportCatalogue& transport_catalogue) {
        transport_catalogue_protobuf::Bus bus_proto;

        bus_proto.set_name(bus.name);

        for (auto stop : bus.stops_on_route) {
            const auto& stops = transport_catalogue.GetStops();
            uint32_t stop_id = FindStopId(stops.cbegin(),
                stops.cend(),
                stop->name);
            bus_proto.add_stops_on_route(stop_id);
        }
        bus_proto.set_is_roundtrip(bus.is_roundtrip);
        bus_proto.set_route_length(bus.route_length);

        return std::move(bus_proto);
    }


    transport_catalogue_protobuf::TransportCatalogue SerializeTransportCatalogue(const transport_catalogue::TransportCatalogue& transport_catalogue) {
        transport_catalogue_protobuf::TransportCatalogue transport_catalogue_proto;
        const auto& stops = transport_catalogue.GetStops();
        const auto& buses = transport_catalogue.GetBuses();
        const auto& distances = transport_catalogue.GetDistance();

        for (const auto& stop : stops) {
            *transport_catalogue_proto.add_stops() = SerializeStops(stop);
        }
        for (const auto& bus : buses) {
            *transport_catalogue_proto.add_buses() = SerializeBuses(bus, transport_catalogue);
        }

        for (const auto [stop_pair, distance] : distances) {
            transport_catalogue_protobuf::Distance& distance_proto = *transport_catalogue_proto.add_distances();
            distance_proto.set_from(FindStopId(stops.cbegin(),
                stops.cend(),
                stop_pair.first->name));
            distance_proto.set_to(FindStopId(stops.cbegin(),
                stops.cend(),
                stop_pair.second->name));
            distance_proto.set_distance(distance);
        }

        return transport_catalogue_proto;
    }

    void DeserializeStop(const transport_catalogue_protobuf::TransportCatalogue& transport_catalogue_proto, transport_catalogue::TransportCatalogue& transport_catalogue) {
        const auto& stops_proto = transport_catalogue_proto.stops();
        for (const auto& stop : stops_proto) {
            domain::Stop tc_stop;
            tc_stop.name = stop.name();
            tc_stop.coordinates.latitude = stop.coordinates().latitude();
            tc_stop.coordinates.longitude = stop.coordinates().longitude();
            transport_catalogue.AddStop(std::move(tc_stop));
        }
    }

    void DeserializeBuses(const transport_catalogue_protobuf::TransportCatalogue& transport_catalogue_proto, transport_catalogue::TransportCatalogue& transport_catalogue) {
        const auto& buses_proto = transport_catalogue_proto.buses();
        for (const auto& bus_proto : buses_proto) {
            domain::Bus tc_bus;
            tc_bus.name = bus_proto.name();
            tc_bus.stops_on_route.reserve(bus_proto.stops_on_route().size());
            for (const auto& stop_id : bus_proto.stops_on_route()) {
                tc_bus.stops_on_route.push_back(transport_catalogue.GetStop(transport_catalogue_proto.stops(stop_id).name()));
            }
            tc_bus.is_roundtrip = bus_proto.is_roundtrip();
            tc_bus.route_length = bus_proto.route_length();

            transport_catalogue.AddBus(std::move(tc_bus));
        }
    }
    void DeserializeDistance(const transport_catalogue_protobuf::TransportCatalogue& transport_catalogue_proto, transport_catalogue::TransportCatalogue& transport_catalogue) {
        const auto& tc_stops = transport_catalogue.GetStops();

        const auto& distances_proto = transport_catalogue_proto.distances();
        for (const auto& distance : distances_proto) {
            const auto& from = transport_catalogue.GetStop(tc_stops[distance.from()].name);
            const auto& to = transport_catalogue.GetStop(tc_stops[distance.to()].name);
            transport_catalogue.AddDistance(from, to, distance.distance());

        }
    }


    transport_catalogue::TransportCatalogue DeserializeTransportCatalogue(const transport_catalogue_protobuf::TransportCatalogue& transport_catalogue_proto) {
        transport_catalogue::TransportCatalogue transport_catalogue;
        DeserializeStop(transport_catalogue_proto, transport_catalogue);
        DeserializeDistance(transport_catalogue_proto, transport_catalogue);
        DeserializeBuses(transport_catalogue_proto, transport_catalogue);

        return transport_catalogue;
    }

    transport_catalogue_protobuf::Color SerializeColor(const svg::Color& tc_color) {

        transport_catalogue_protobuf::Color color_proto;

        if (std::holds_alternative<std::monostate>(tc_color)) {
            color_proto.set_none(true);

        }
        else if (std::holds_alternative<svg::Rgb>(tc_color)) {
            svg::Rgb rgb = std::get<svg::Rgb>(tc_color);

            color_proto.mutable_rgb()->set_red_(rgb.red_);
            color_proto.mutable_rgb()->set_green_(rgb.green_);
            color_proto.mutable_rgb()->set_blue_(rgb.blue_);

        }
        else if (std::holds_alternative<svg::Rgba>(tc_color)) {
            svg::Rgba rgba = std::get<svg::Rgba>(tc_color);

            color_proto.mutable_rgba()->set_red_(rgba.red_);
            color_proto.mutable_rgba()->set_green_(rgba.green_);
            color_proto.mutable_rgba()->set_blue_(rgba.blue_);
            color_proto.mutable_rgba()->set_opacity_(rgba.opacity_);

        }
        else if (std::holds_alternative<std::string>(tc_color)) {
            color_proto.set_string_color(std::get<std::string>(tc_color));
        }

        return color_proto;
    }

    svg::Color DeserializeColor(const transport_catalogue_protobuf::Color& color_proto) {

        svg::Color color;

        if (color_proto.has_rgb()) {
            svg::Rgb rgb;

            rgb.red_ = color_proto.rgb().red_();
            rgb.green_ = color_proto.rgb().green_();
            rgb.blue_ = color_proto.rgb().blue_();

            color = rgb;

        }
        else if (color_proto.has_rgba()) {
            svg::Rgba rgba;

            rgba.red_ = color_proto.rgba().red_();
            rgba.green_ = color_proto.rgba().green_();
            rgba.blue_ = color_proto.rgba().blue_();
            rgba.opacity_ = color_proto.rgba().opacity_();

            color = rgba;

        }
        else { color = color_proto.string_color(); }

        return color;
    }

    transport_catalogue_protobuf::RenderSettings SerializeRenderSettings(const map_renderer::RenderSettings& render_settings) {

        transport_catalogue_protobuf::RenderSettings render_settings_proto;

        render_settings_proto.set_width_(render_settings.width_);
        render_settings_proto.set_height_(render_settings.height_);
        render_settings_proto.set_padding_(render_settings.padding_);
        render_settings_proto.set_line_width_(render_settings.line_width_);
        render_settings_proto.set_stop_radius_(render_settings.stop_radius_);
        render_settings_proto.set_bus_label_font_size_(render_settings.bus_label_font_size_);

        transport_catalogue_protobuf::Point bus_label_offset_proto;
        bus_label_offset_proto.set_x(render_settings.bus_label_offset_.first);
        bus_label_offset_proto.set_y(render_settings.bus_label_offset_.second);

        *render_settings_proto.mutable_bus_label_offset_() = std::move(bus_label_offset_proto);

        render_settings_proto.set_stop_label_font_size_(render_settings.stop_label_font_size_);

        transport_catalogue_protobuf::Point stop_label_offset_proto;
        stop_label_offset_proto.set_x(render_settings.stop_label_offset_.first);
        stop_label_offset_proto.set_y(render_settings.stop_label_offset_.second);

        *render_settings_proto.mutable_stop_label_offset_() = std::move(stop_label_offset_proto);
        *render_settings_proto.mutable_underlayer_color_() = std::move(SerializeColor(render_settings.underlayer_color_));
        render_settings_proto.set_underlayer_width_(render_settings.underlayer_width_);

        const auto& colors = render_settings.color_palette_;
        for (const auto& color : colors) {
            *render_settings_proto.add_color_palette_() = std::move(SerializeColor(color));
        }

        return render_settings_proto;
    }

    map_renderer::RenderSettings DeserializeRenderSettings(const transport_catalogue_protobuf::RenderSettings& render_settings_proto) {

        map_renderer::RenderSettings render_settings;

        render_settings.width_ = render_settings_proto.width_();
        render_settings.height_ = render_settings_proto.height_();
        render_settings.padding_ = render_settings_proto.padding_();
        render_settings.line_width_ = render_settings_proto.line_width_();
        render_settings.stop_radius_ = render_settings_proto.stop_radius_();
        render_settings.bus_label_font_size_ = render_settings_proto.bus_label_font_size_();

        render_settings.bus_label_offset_.first = render_settings_proto.bus_label_offset_().x();
        render_settings.bus_label_offset_.second = render_settings_proto.bus_label_offset_().y();

        render_settings.stop_label_font_size_ = render_settings_proto.stop_label_font_size_();

        render_settings.stop_label_offset_.first = render_settings_proto.stop_label_offset_().x();
        render_settings.stop_label_offset_.second = render_settings_proto.stop_label_offset_().y();

        render_settings.underlayer_color_ = DeserializeColor(render_settings_proto.underlayer_color_());
        render_settings.underlayer_width_ = render_settings_proto.underlayer_width_();

        for (const auto& color_proto : render_settings_proto.color_palette_()) {
            render_settings.color_palette_.push_back(DeserializeColor(color_proto));
        }

        return render_settings;
    }

    transport_catalogue_protobuf::RoutingSettings SerializeRoutingSettings(const domain::RoutingSettings& routing_settings) {

        transport_catalogue_protobuf::RoutingSettings routing_settings_proto;

        routing_settings_proto.set_bus_wait_time(routing_settings.bus_wait_time);
        routing_settings_proto.set_bus_velocity(routing_settings.bus_velocity);

        return routing_settings_proto;
    }

    domain::RoutingSettings DeserializeRoutingSettings(const transport_catalogue_protobuf::RoutingSettings& routing_settings_proto) {

        domain::RoutingSettings routing_settings;

        routing_settings.bus_wait_time = routing_settings_proto.bus_wait_time();
        routing_settings.bus_velocity = routing_settings_proto.bus_velocity();

        return routing_settings;
    }

    void SerializeCatalogue(const transport_catalogue::TransportCatalogue& transport_catalogue,
        const map_renderer::RenderSettings& render_settings,
        const domain::RoutingSettings& routing_settings,
        std::ostream& out) {

        transport_catalogue_protobuf::Catalogue catalogue_proto;

        transport_catalogue_protobuf::TransportCatalogue transport_catalogue_proto = SerializeTransportCatalogue(transport_catalogue);
        transport_catalogue_protobuf::RenderSettings render_settings_proto = SerializeRenderSettings(render_settings);
        transport_catalogue_protobuf::RoutingSettings routing_settings_proto = SerializeRoutingSettings(routing_settings);

        *catalogue_proto.mutable_transport_catalogue() = std::move(transport_catalogue_proto);
        *catalogue_proto.mutable_render_settings() = std::move(render_settings_proto);
        *catalogue_proto.mutable_routing_settings() = std::move(routing_settings_proto);

        catalogue_proto.SerializePartialToOstream(&out);

    }

    Catalogue DeserializeCatalogue(std::istream& in) {

        transport_catalogue_protobuf::Catalogue catalogue_proto;
        auto success_parsing_catalogue_from_istream = catalogue_proto.ParseFromIstream(&in);

        if (!success_parsing_catalogue_from_istream) {
            throw std::runtime_error("error");
        }

        return { DeserializeTransportCatalogue(catalogue_proto.transport_catalogue()),
                DeserializeRenderSettings(catalogue_proto.render_settings()),
                DeserializeRoutingSettings(catalogue_proto.routing_settings()) };
    }

}//end namespace serialization