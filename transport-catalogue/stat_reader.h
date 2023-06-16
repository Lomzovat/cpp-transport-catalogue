#pragma once
#include "transport_catalogue.h"
#include <algorithm>
#include <vector>

namespace transport_catalogue {
    namespace detail {

        namespace stop {

            void QueryStop(TransportCatalogue& catalogue, std::string_view stop_name);
        }

        namespace bus {

            void QueryBus(TransportCatalogue& catalogue, std::string_view str);
        }

        void ParseQuery(TransportCatalogue& catalogue, std::string_view str);

        void OutputQuery(TransportCatalogue& catalogue);

    }
}