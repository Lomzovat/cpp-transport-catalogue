#pragma once

#include "json.h"
#include "transport_catalogue.h"
#include "map_renderer.h"

using namespace json;
using namespace transport_catalogue;

//выполняет разбор JSON-данных, построенных в ходе парсинга, и формирует массив //JSON-ответов;


class JsonReader {
public:
	JsonReader() = default;
	JsonReader(Document doc);
	JsonReader(std::istream& input);
	Stop ParseNodeStop(Node& node);
	Bus ParseNodeBus(Node& node, TransportCatalogue& catalogue);

	void ParseBaseRequest(const Node& root, TransportCatalogue& catalogue);
	void ParseStatRequest(const Node& root, std::vector<QueryStat>& stat_request);
	void ParseRenderRequest(const Node& node, map_renderer::RenderSettings& render_settings);

	void ParseNode(const Node& root, TransportCatalogue& catalogue,
		std::vector<QueryStat>& stat_request,
		map_renderer::RenderSettings& render_settings);

	void Parse(TransportCatalogue& catalogue,
		std::vector<QueryStat>& stat_request,
		map_renderer::RenderSettings& render_settings);

	DistanceToStop ParseNodeDistance(Node& node, TransportCatalogue& catalogue);

	const Document& GetDocument() const;

private:
	Document document_;
};