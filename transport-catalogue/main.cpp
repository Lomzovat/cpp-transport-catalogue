#include "transport_catalogue.h"
#include "input_reader.h"
#include "stat_reader.h"

using namespace std;
using namespace transport_catalogue;
using namespace transport_catalogue::detail;


int main() {
    TransportCatalogue catalogue;
    ParseInputQuery(std::cin, catalogue);
    ReadOutputQuery(std::cin, catalogue);
}