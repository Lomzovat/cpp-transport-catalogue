#include "stat_reader.h"

namespace transport_catalogue {
    namespace detail {

        namespace stop {

            void QueryStop(TransportCatalogue& catalogue, std::string_view stop_name) {
                auto first_stop = 5;
                stop_name = stop_name.substr(first_stop);
                std::unordered_set<const Bus*> unique_buses;
                std::unordered_set<std::string_view> unique_buses_name;
                std::vector <std::string> bus_name_;

                Stop* stop = catalogue.GetStop(stop_name);

                if (stop != NULL) {
                    unique_buses = catalogue.stop_get_uniq_buses(stop);

                    if (unique_buses.size() == 0) {
                        std::cout << "Stop " << stop_name << ": no buses" << std::endl;
                    }
                    else {
                        std::cout << "Stop " << stop_name << ": buses ";

                        for (const Bus* _bus : unique_buses) {
                            bus_name_.push_back(_bus->name);
                        }

                        std::sort(bus_name_.begin(), bus_name_.end());

                        for (std::string_view _bus_name : bus_name_) {
                            std::cout << _bus_name;
                            std::cout << " ";
                        }
                        std::cout << std::endl;
                    }
                }
                else {
                    std::cout << "Stop " << stop_name << ": not found" << std::endl;
                }
            }
        }


        namespace bus {

            void QueryBus(TransportCatalogue& catalogue, std::string_view str) {
                auto first_bus = 4;
                str = str.substr(first_bus);

                Bus* bus = catalogue.GetBus(str);
                if (bus != nullptr) {
                    std::cout << "Bus " << bus->name << ": "
                        << bus->stops.size() << " stops on route, "
                        << (catalogue.get_uniq_stops(bus)).size() << " unique stops, "
                        << catalogue.get_distance_to_bus(bus) << " route length, "
                        << std::setprecision(6) << double(catalogue.get_distance_to_bus(bus)
                            / catalogue.get_length(bus))
                        << " curvature" << std::endl;
                }
                else {
                    std::cout << "Bus " << str << ": not found" << std::endl;
                }
            }
        }


        void ParseQuery(TransportCatalogue& catalogue, std::string_view str) {
            if (str.substr(0, 3) == "Bus") {
                bus::QueryBus(catalogue, str);
            }
            else if (str.substr(0, 4) == "Stop") {
                stop::QueryStop(catalogue, str);
            }
            else {
                std::cout << "Error query" << std::endl;
            }
        }

        void OutputQuery(TransportCatalogue& catalogue) {
            std::string count;
            std::getline(std::cin, count);

            std::string str;
            std::vector<std::string> query;
            auto num = stoi(count);

            for (int i = 0; i < num; ++i) {
                std::getline(std::cin, str);
                query.push_back(str);
            }

            for (auto& st : query) {
                ParseQuery(catalogue, st);
            }
        }

    }
}