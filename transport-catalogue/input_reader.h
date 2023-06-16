#pragma once
#include "transport_catalogue.h"

namespace transport_catalogue {
    namespace detail {

        namespace stop {

            Stop ParsingStop(std::string_view str);
        }

        namespace bus {
            Bus ParsingBus(TransportCatalogue& catalogue, std::string_view str);
        }

        namespace distance {
            std::vector<Distance> parsing_distance(std::string str, TransportCatalogue& catalogue);
        }

        void InputQuery(TransportCatalogue& catalogue);

    }
}