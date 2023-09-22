#pragma once

#include "transport_catalogue.h"
#include "transport_catalogue.pb.h"

#include "svg.pb.h"

#include "map_renderer.h"
#include "map_renderer.pb.h"

#include "transport_router.h"
#include "transport_router.pb.h"

#include <iostream>

namespace serialization {

    using namespace transport_catalogue;

    struct SerializationSettings { std::string file_name; };

    struct Catalogue {
        transport_catalogue::TransportCatalogue transport_catalogue_;
        map_renderer::RenderSettings render_settings_;
        domain::RoutingSettings routing_settings_;
    };

    template <typename It>
    uint32_t CalculateId(It start, It end, std::string_view name);

    transport_catalogue_protobuf::TransportCatalogue TransportCatalogueSerialization(const transport_catalogue::TransportCatalogue& transport_catalogue);
    transport_catalogue::TransportCatalogue TransportCatalogueDeserialization(const transport_catalogue_protobuf::TransportCatalogue& transport_catalogue_proto);

    transport_catalogue_protobuf::Color ColorSerialization(const svg::Color& tc_color);
    svg::Color ColorDeserialization(const transport_catalogue_protobuf::Color& color_proto);

    transport_catalogue_protobuf::RenderSettings RenderSettingsSerialization(const map_renderer::RenderSettings& render_settings);
    map_renderer::RenderSettings RenderSettingsDeserialization(const transport_catalogue_protobuf::RenderSettings& render_settings_proto);

    transport_catalogue_protobuf::RoutingSettings RoutingSettingsSerialization(const domain::RoutingSettings& routing_settings);
    domain::RoutingSettings RoutingSettingsDeserialization(const transport_catalogue_protobuf::RoutingSettings& routing_settings_proto);

    void CatalogueSerialization(const transport_catalogue::TransportCatalogue& transport_catalogue,
        const map_renderer::RenderSettings& render_settings,
        const domain::RoutingSettings& routing_settings,
        std::ostream& out);

    Catalogue CatalogueDeserialization(std::istream& in);

}//end namespace serialization
