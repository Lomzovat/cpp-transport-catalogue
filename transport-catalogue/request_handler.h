#pragma once

#include "transport_catalogue.h"
#include "map_renderer.h"
#include "json.h"

//oбрабатывает запросы. Играет роль Фасада, который упрощает взаимодействие с транспортным каталогом;

using namespace transport_catalogue;
using namespace map_renderer;
using namespace json;


namespace request_handler {


    class RequestHandler {
    public:

        std::vector<geo::Coordinates> GetStopCoordinates(TransportCatalogue& catalog) const;
        RequestHandler() = default;

        BusStat GetBusStat(TransportCatalogue& catalogue, std::string_view bus_name);
        StopStat GetStopStat(TransportCatalogue& catalogue, std::string_view stop_name);

        Node GetNodeBus(int id_request, BusStat bus_info);
        Node GetNodeStop(int id_request, StopStat stop_info);
        Node GetNodeMap(int id_request, TransportCatalogue& catalog, RenderSettings render_settings);

        std::vector<std::string_view> GetSortedBusesNames(TransportCatalogue& catalog) const;


        void ParseMapRender(MapRenderer& map_catalogue, TransportCatalogue& catalogue) const;

        void ParseQuery(TransportCatalogue& catalogue, std::vector<QueryStat>& stat_requests, RenderSettings& render_settings);

        const Document& GetDocument() const;

    private:
        Document document_out;

    };
}//end namespace request_handler
