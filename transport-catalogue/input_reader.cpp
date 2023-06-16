#include "input_reader.h"
#include "stat_reader.h"
#include <algorithm>

namespace transport_catalogue {
    namespace detail {

        namespace stop {

            Stop ParsingStop(std::string str) {
                auto first_stop_word = 5;
                auto colon_pos = str.find(':');
                auto dist_to_next = 2;
                auto comma_pos = str.find(',');

                Stop stop;

                stop.name = str.substr(first_stop_word, colon_pos - first_stop_word);
                stop.latitude = stod(str.substr(colon_pos + dist_to_next, comma_pos - colon_pos - dist_to_next));
                stop.longitude = stod(str.substr(comma_pos + dist_to_next));

                return stop;
            }
        }

        namespace bus {

            Bus ParsingBus(TransportCatalogue& catalogue, std::string_view str) {
                auto first_bus_word = 4;
                auto colon_pos = str.find(':');
                auto dist_to_next = 2;

                Bus bus;
                bus.name = str.substr(first_bus_word, colon_pos - first_bus_word);

                str = str.substr(colon_pos + dist_to_next);

                auto more_pos = str.find('>');
                if (more_pos == std::string_view::npos) {
                    auto dash_pos = str.find('-');

                    while (dash_pos != std::string_view::npos) {
                        bus.stops.push_back(catalogue.GetStop(str.substr(0, dash_pos - 1)));

                        str = str.substr(dash_pos + dist_to_next);
                        dash_pos = str.find('-');
                    }

                    bus.stops.push_back(catalogue.GetStop(str.substr(0, dash_pos - 1)));
                    size_t size_ = bus.stops.size() - 1;

                    for (size_t i = size_; i > 0; i--) {
                        bus.stops.push_back(bus.stops[i - 1]);
                    }

                }
                else {
                    while (more_pos != std::string_view::npos) {
                        bus.stops.push_back(catalogue.GetStop(str.substr(0, more_pos - 1)));

                        str = str.substr(more_pos + dist_to_next);
                        more_pos = str.find('>');
                    }

                    bus.stops.push_back(catalogue.GetStop(str.substr(0, more_pos - 1)));
                }
                return bus;
            }
        }

        namespace distance {

            std::vector<Distance> parsing_distance(std::string str, TransportCatalogue& catalogue) {

                std::vector<Distance> distances_;
                auto first_stop_word = 5;
                auto colon_pos = str.find(':');
                auto dist_to_next = 2;
                auto remove_to = 5;

                std::string name_ = str.substr(first_stop_word,
                    colon_pos - first_stop_word);
                str = str.substr(str.find(',') + 1);
                str = str.substr(str.find(',') + dist_to_next);

                while (str.find(',') != std::string::npos) {


                    int distance = stoi(str.substr(0, str.find('m')));
                    std::string stop_dist_name = str.substr(str.find('m') + remove_to);
                    stop_dist_name = stop_dist_name.substr(0, stop_dist_name.find(','));

                    distances_.push_back({ catalogue.GetStop(name_), catalogue.GetStop(stop_dist_name), distance });

                    str = str.substr(str.find(',') + dist_to_next);
                }
                std::string last_name = str.substr(str.find('m') + remove_to);
                int distance = stoi(str.substr(0, str.find('m')));

                distances_.push_back({ catalogue.GetStop(name_), catalogue.GetStop(last_name), distance });
                return distances_;
            }
        }

        void InputQuery(TransportCatalogue& catalogue) {

            std::string count;
            std::getline(std::cin, count);

            if (count != "") {
                std::string str;
                std::vector<std::string> buses;
                std::vector<std::string> stops;
                int amount = stoi(count);
                auto bus_distance = 3;

                for (int i = 0; i < amount; ++i) {
                    std::getline(std::cin, str);

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
                    catalogue.AddStop(stop::ParsingStop(stop));
                }

                for (auto bus : buses) {
                    catalogue.AddBus(bus::ParsingBus(catalogue, bus));
                }
                for (auto stop : stops) {
                    catalogue.AddDistance(distance::parsing_distance(stop, catalogue));
                }
            }