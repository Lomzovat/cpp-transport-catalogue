#include "json_reader.h"
#include "map_renderer.h"
#include "request_handler.h"

using namespace std;
using namespace transport_catalogue;
using namespace map_renderer;
using namespace request_handler;
using namespace domain;

int main() {

    vector<QueryStat> stat_request;
    RenderSettings render_settings;
    TransportCatalogue catalogue;

    JsonReader json_reader;
    RequestHandler request_handler;

    json_reader = JsonReader(cin);
    json_reader.Parse(catalogue, stat_request, render_settings);

    request_handler = RequestHandler();
    request_handler.ParseQuery(catalogue, stat_request, render_settings);

    json::Print(request_handler.GetDocument(), cout);
}