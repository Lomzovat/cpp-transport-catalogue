#include "input_reader.h"
#include "stat_reader.h"
#include <algorithm>

namespace transport_catalogue {
    namespace detail {
        namespace stop {



            Stop ParseStop(std::string str) {
                auto first_stop_word = 5;
                auto colon_pos = str.find(':');
                auto dist_to_next = 2;
                auto comma_pos = str.find(',');

                Stop stop;

                stop.stop_name = str.substr(first_stop_word, colon_pos - first_stop_word);
                stop.coordinates.latitude = stod(str.substr(colon_pos + dist_to_next,
                    comma_pos - colon_pos - dist_to_next));
                stop.coordinates.longitude = stod(str.substr(comma_pos + dist_to_next));

                return stop;
            }
        }

        namespace bus {


            Bus ParseBus(TransportCatalogue& catalogue, std::string_view str) {
                auto first_bus_word = 4;
                auto colon_pos = str.find(':');
                auto dist_to_next = 2;

                Bus bus;
                bus.bus_name = str.substr(first_bus_word,
                    colon_pos - first_bus_word);

                str = str.substr(colon_pos + dist_to_next);

                auto more_pos = str.find('>');
                if (more_pos == std::string_view::npos) {
                    auto dash_pos = str.find('-');

                    while (dash_pos != std::string_view::npos) {
                        bus.stops_on_route.push_back(catalogue.GetStop(str.substr(0, dash_pos - 1)));

                        str = str.substr(dash_pos + dist_to_next);
                        dash_pos = str.find('-');
                    }

                    bus.stops_on_route.push_back(catalogue.GetStop(str.substr(0, dash_pos - 1)));
                    size_t size_ = bus.stops_on_route.size() - 1;

                    for (size_t i = size_; i > 0; i--) {
                        bus.stops_on_route.push_back(bus.stops_on_route[i - 1]);
                    }

                }
                else {
                    while (more_pos != std::string_view::npos) {
                        bus.stops_on_route.push_back(catalogue.GetStop(str.substr(0, more_pos - 1)));

                        str = str.substr(more_pos + dist_to_next);
                        more_pos = str.find('>');
                    }

                    bus.stops_on_route.push_back(catalogue.GetStop(str.substr(0, more_pos - 1)));
                }
                return bus;
            }
        }

        namespace distance {
            DistanceToStop ParseDistance(std::string str, TransportCatalogue& catalogue) {

                DistanceToStop distances_;
                auto first_stop_word = 5;
                auto colon_pos = str.find(':');
                auto dist_to_next = 2;

                std::string name_ = str.substr(first_stop_word,
                    colon_pos - first_stop_word);
                str = str.substr(str.find(',') + 1);
                str = str.substr(str.find(',') + dist_to_next);

                while (str.find(',') != std::string::npos) {


                    int distance = stoi(str.substr(0, str.find('m')));
                    std::string stop_dist_name = str.substr(str.find('m') + first_stop_word);
                    stop_dist_name = stop_dist_name.substr(0, stop_dist_name.find(','));

                    distances_[std::make_pair(catalogue.GetStop(name_), catalogue.GetStop(stop_dist_name))] = distance;

                    str = str.substr(str.find(',') + dist_to_next);
                }
                std::string last_name = str.substr(str.find('m') + first_stop_word);
                int distance = stoi(str.substr(0, str.find('m')));

                distances_[std::make_pair(catalogue.GetStop(name_), catalogue.GetStop(last_name))] = distance;
                return distances_;
            }
        }


        void ParseInputQuery(std::istream& input, TransportCatalogue& catalogue) {

            std::string count;
            std::getline(input, count);

            if (count != "") {
                std::string str;
                std::vector<std::string> buses;
                std::vector<std::string> stops;
                int amount = stoi(count);
                auto bus_distance = 3;

                for (int i = 0; i < amount; ++i) {
                    std::getline(input, str);

                    if (str != "") {
                        auto space_pos = str.find_first_not_of(' ');
                        str = str.substr(space_pos);

                        if (str.substr(0, bus_distance) != "Bus") {
                            stops.push_back(str);
                        }
                        else {
                            buses.push_back(str);
                        }
                    }
                }

                for (auto stop : stops) {
                    catalogue.AddStop(stop::ParseStop(stop));
                }
                for (auto stop : stops) {
                    catalogue.AddDistance(distance::ParseDistance(stop, catalogue));
                }

                for (auto bus : buses) {
                    catalogue.AddBus(bus::ParseBus(catalogue, bus));
                }
            }
        }
    }
}