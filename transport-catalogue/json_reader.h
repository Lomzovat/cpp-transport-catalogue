#pragma once

#include "json.h"
#include "transport_catalogue.h"
#include "map_renderer.h"


using namespace transport_catalogue;


namespace json {
	//��������� ������ JSON-������, ����������� � ���� ��������, � ��������� ������ //JSON-�������;


	class JsonReader {
	public:
		JsonReader() = default;
		JsonReader(Document doc);
		JsonReader(std::istream& input);
		Stop ParseNodeStop(Node& node);
		Bus ParseNodeBus(Node& node, TransportCatalogue& catalogue);

		svg::Color WorkWithColor(Array& arr_color);

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
}// end namespace json