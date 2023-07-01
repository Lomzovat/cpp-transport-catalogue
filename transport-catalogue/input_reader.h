#pragma once
#include "transport_catalogue.h"

namespace transport_catalogue {
	namespace detail {
		namespace stop {
			Stop ParseStop(std::string_view str);

		}
		namespace bus {
			Bus ParseBus(TransportCatalogue& catalogue, std::string_view str);
		}
		namespace distance {
			DistanceToStop ParseDistance(std::string str, TransportCatalogue& catalogue);
		}
		void ParseInputQuery(std::istream& input, TransportCatalogue& catalogue);
	}
}