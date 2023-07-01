#pragma once 

#include "transport_catalogue.h" 
#include <algorithm> 
#include <vector> 



namespace transport_catalogue {
    namespace detail {

        namespace stop {
            void PrintQueryStop(TransportCatalogue& catalogue);
        }

        namespace bus {
            void PrintQueryBus(TransportCatalogue& catalogue, std::string_view str);
        }

        void PrintParseQuery(TransportCatalogue& catalogue, std::string_view str);

        void ReadOutputQuery(std::istream& input, TransportCatalogue& catalogue);

    }
}