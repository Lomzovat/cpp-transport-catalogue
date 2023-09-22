#pragma once
#include "json.h"
#include "serialization.h"
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "transport_router.h"
#include "router.h"

using namespace transport_catalogue;
using namespace serialization;

namespace json {

    class JsonReader {
    public:
        JsonReader() = default;
        JsonReader(Document doc);
        JsonReader(std::istream& input);

        void ParseBaseRequest(const Node& root, TransportCatalogue& catalogue);
        void ParseStatRequest(const Node& root, std::vector<QueryStat>& stat_request);
        void ParseRenderRequest(const Node& node, map_renderer::RenderSettings& render_settings);

        svg::Color WorkWithColor(Array& arr_color);

        void ParseRouting(const Node& node, router::RoutingSettings& route_set);
        void ParseSerialization(const Node& node, serialization::SerializationSettings& serialization_set);


        void ParseNode(TransportCatalogue& catalogue,
            map_renderer::RenderSettings& render_settings,
            router::RoutingSettings& routing_settings,
            serialization::SerializationSettings& serialization_settings);

        void ParseRequest(std::vector<QueryStat>& stat_request,
            serialization::SerializationSettings& serialization_settings);

        Stop ParseNodeStop(Node& node);
        Bus ParseNodeBus(Node& node, TransportCatalogue& catalogue);
        DistanceToStop ParseNodeDistance(Node& node, TransportCatalogue& catalogue);

        const Document& GetDocument() const;

    private:
        Document document_;
    };

}//end namespace json
