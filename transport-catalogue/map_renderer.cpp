#include "map_renderer.h"

using namespace std::literals;

namespace map_renderer {

    bool SphereProjector::IsZero(double value) {
        return std::abs(value) < EPSILON;
    }

    MapRenderer::MapRenderer(RenderSettings& render_settings) :
        render_settings_(render_settings)
    {}

    svg::Point SphereProjector::operator()(geo::Coordinates coords) const {
        return { (coords.longitude - min_lon_) * zoom_coeff_ + padding_,
                (max_lat_ - coords.latitude) * zoom_coeff_ + padding_ };
    }

    SphereProjector MapRenderer::GetSphereProjector(const std::vector<geo::Coordinates>& points) const {
        return SphereProjector(points.begin(),
            points.end(),
            render_settings_.width_,
            render_settings_.height_,
            render_settings_.padding_);
    }

    void MapRenderer::RenderSphereProjector(std::vector<geo::Coordinates> points) {
        sphere_projector = SphereProjector(points.begin(),
            points.end(),
            render_settings_.width_,
            render_settings_.height_,
            render_settings_.padding_);
    }

    RenderSettings MapRenderer::GetRenderSettings() const {
        return render_settings_;
    }

    int MapRenderer::GetPaletteSize() const {
        return render_settings_.color_palette_.size();
    }


    svg::Color MapRenderer::GetColor(int line_number) const {
        return render_settings_.color_palette_[line_number];
    }

    void MapRenderer::RenderLineProperties(svg::Polyline& polyline,
        int line_number) const {

        polyline.SetStrokeColor(GetColor(line_number));
        polyline.SetFillColor("none"s);
        polyline.SetStrokeWidth(render_settings_.line_width_);
        polyline.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
        polyline.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
    }

    void MapRenderer::RenderBusTextProperties(svg::Text& text,
        const std::string& name,
        svg::Point position) const {

        text.SetPosition(position);
        text.SetOffset({ render_settings_.bus_label_offset_.first,
                         render_settings_.bus_label_offset_.second });
        text.SetFontSize(render_settings_.bus_label_font_size_);
        text.SetFontFamily("Verdana");
        text.SetFontWeight("bold");
        text.SetData(name);
    }

    void MapRenderer::RenderBusTextAddProperties(svg::Text& text,
        const std::string& name,
        svg::Point position) const {
        RenderBusTextProperties(text,
            name,
            position);

        text.SetFillColor(render_settings_.underlayer_color_);
        text.SetStrokeColor(render_settings_.underlayer_color_);
        text.SetStrokeWidth(render_settings_.underlayer_width_);
        text.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
        text.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
    }

    void MapRenderer::RenderBusTextColorProperties(svg::Text& text,
        const std::string& name,
        int palette,
        svg::Point position) const {
        RenderBusTextProperties(text,
            name,
            position);

        text.SetFillColor(GetColor(palette));
    }

    void MapRenderer::RenderStopsCirclesProperties(svg::Circle& circle,
        svg::Point position) const {

        circle.SetCenter(position);
        circle.SetRadius(render_settings_.stop_radius_);
        circle.SetFillColor("white");
    }

    void MapRenderer::RenderStopsTextProperties(svg::Text& text,
        const std::string& name,
        svg::Point position) const {


        text.SetPosition(position);
        text.SetOffset({ render_settings_.stop_label_offset_.first,
                         render_settings_.stop_label_offset_.second });
        text.SetFontSize(render_settings_.stop_label_font_size_);
        text.SetFontFamily("Verdana");
        text.SetData(name);
    }

    void MapRenderer::RenderStopsTextAddProperties(svg::Text& text,
        const std::string& name,
        svg::Point position) const {

        RenderStopsTextProperties(text,
            name,
            position);

        text.SetFillColor(render_settings_.underlayer_color_);
        text.SetStrokeColor(render_settings_.underlayer_color_);
        text.SetStrokeWidth(render_settings_.underlayer_width_);
        text.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
        text.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
    }

    void MapRenderer::RenderStopsTextColorProperties(svg::Text& text,
        const std::string& name,
        svg::Point position) const {


        RenderStopsTextProperties(text,
            name,
            position);
        text.SetFillColor("black");
    }

    void MapRenderer::AddLine(std::vector<std::pair<Bus*, int>>& buses_palette) {
        std::vector<geo::Coordinates> stops_geo_coords;

        for (auto [bus, palette] : buses_palette) {

            for (Stop* stop : bus->stops_on_route) {
                geo::Coordinates coordinates;
                coordinates.latitude = stop->coordinates.latitude;
                coordinates.longitude = stop->coordinates.longitude;

                stops_geo_coords.push_back(coordinates);
            }

            svg::Polyline bus_line;
            bool bus_empty = true;

            for (auto& coord : stops_geo_coords) {
                bus_empty = false;
                bus_line.AddPoint(sphere_projector(coord));
            }

            if (!bus_empty) {
                RenderLineProperties(bus_line,
                    palette);
                map_svg.Add(bus_line);
            }

            stops_geo_coords.clear();
        }
    }

    void MapRenderer::AddBusRoute(std::vector<std::pair<Bus*, int>>& buses_palette) {
        std::vector<geo::Coordinates> stops_geo_coords;
        bool bus_empty = true;

        for (auto [bus, palette] : buses_palette) {

            for (Stop* stop : bus->stops_on_route) {
                geo::Coordinates coordinates;
                coordinates.latitude = stop->coordinates.latitude;
                coordinates.longitude = stop->coordinates.longitude;

                stops_geo_coords.push_back(coordinates);

                if (bus_empty) bus_empty = false;
            }

            svg::Text route_name_roundtrip;
            svg::Text route_title_roundtrip;
            svg::Text route_name_notroundtrip;
            svg::Text route_title_notroundtrip;

            if (!bus_empty) {

                if (bus->is_roundtrip) {
                    RenderBusTextAddProperties(route_name_roundtrip,
                        std::string(bus->name),
                        sphere_projector(stops_geo_coords[0]));
                    map_svg.Add(route_name_roundtrip);

                    RenderBusTextColorProperties(route_title_roundtrip,
                        std::string(bus->name),
                        palette,
                        sphere_projector(stops_geo_coords[0]));
                    map_svg.Add(route_title_roundtrip);

                }
                else {
                    RenderBusTextAddProperties(route_name_roundtrip,
                        std::string(bus->name),
                        sphere_projector(stops_geo_coords[0]));
                    map_svg.Add(route_name_roundtrip);

                    RenderBusTextColorProperties(route_title_roundtrip,
                        std::string(bus->name),
                        palette,
                        sphere_projector(stops_geo_coords[0]));
                    map_svg.Add(route_title_roundtrip);

                    if (stops_geo_coords[0] != stops_geo_coords[stops_geo_coords.size() / 2]) {
                        RenderBusTextAddProperties(route_name_notroundtrip,
                            std::string(bus->name),
                            sphere_projector(stops_geo_coords[stops_geo_coords.size() / 2]));
                        map_svg.Add(route_name_notroundtrip);

                        RenderBusTextColorProperties(route_title_notroundtrip,
                            std::string(bus->name),
                            palette,
                            sphere_projector(stops_geo_coords[stops_geo_coords.size() / 2]));
                        map_svg.Add(route_title_notroundtrip);
                    }
                }
            }

            bus_empty = false;
            stops_geo_coords.clear();
        }
    }

    void MapRenderer::AddStopCircle(std::vector<Stop*>& stops) {
        std::vector<geo::Coordinates> stops_geo_coords;
        svg::Circle icon;

        for (Stop* stop_info : stops) {

            if (stop_info) {
                geo::Coordinates coordinates;
                coordinates.latitude = stop_info->coordinates.latitude;
                coordinates.longitude = stop_info->coordinates.longitude;

                RenderStopsCirclesProperties(icon,
                    sphere_projector(coordinates));
                map_svg.Add(icon);
            }
        }
    }

    void MapRenderer::AddStopName(std::vector<Stop*>& stops) {
        std::vector<geo::Coordinates> stops_geo_coords;

        svg::Text svg_stop_name;
        svg::Text svg_stop_name_title;

        for (Stop* stop_info : stops) {

            if (stop_info) {
                geo::Coordinates coordinates;
                coordinates.latitude = stop_info->coordinates.latitude;
                coordinates.longitude = stop_info->coordinates.longitude;

                RenderStopsTextAddProperties(svg_stop_name,
                    stop_info->name,
                    sphere_projector(coordinates));
                map_svg.Add(svg_stop_name);

                RenderStopsTextColorProperties(svg_stop_name_title,
                    stop_info->name,
                    sphere_projector(coordinates));
                map_svg.Add(svg_stop_name_title);
            }
        }
    }

    std::vector<std::string_view> MapRenderer::GetSortedBusesNames(TransportCatalogue& catalog) const {
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

    void MapRenderer::ParseMapRender(MapRenderer& map_catalogue, TransportCatalogue& catalogue) const {
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

            for (auto& [stop_name, stop] : stops) {

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

    void MapRenderer::GetMapStream(std::ostream& stream_) {
        map_svg.Render(stream_);
    }

}//end namespace map_renderer
