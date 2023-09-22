#pragma once

#include "transport_catalogue.h"
#include "map_renderer.h"
#include "json_builder.h"
#include "transport_router.h"

using namespace transport_catalogue;
using namespace map_renderer;
using namespace json;
using namespace json::builder;
using namespace router;

namespace request_handler {

    class RequestHandler {
    public:

        RequestHandler() = default;

        Node GetNodeStop(int id_request, const StopStat& query_result);
        Node GetNodeBus(int id_request, const BusStat& query_result);
        Node GetNodeMap(int id_request, TransportCatalogue& catalogue, RenderSettings render_settings);
        Node GetNodeRoute(QueryStat& request, TransportCatalogue& catalogue, TransportRouter& routing);

        std::optional<RouteInfo> GetRouterInfo(std::string_view start,
            std::string_view end,
            TransportCatalogue& catalogue,
            TransportRouter& routing) const;

        std::vector<geo::Coordinates> GetStopCoordinates(TransportCatalogue& catalogue_) const;


        BusStat GetBusStat(TransportCatalogue& catalogue, std::string_view str);
        StopStat GetStopStat(TransportCatalogue& catalogue, std::string_view stop_name);

        void ParseQuery(TransportCatalogue& catalogue,
            std::vector<QueryStat>& stat_requests,
            RenderSettings& render_settings,
            RoutingSettings& route_settings);



        const Document& GetDocument() const;

    private:
        Document document_out;
    };

}//end namespace request_handler