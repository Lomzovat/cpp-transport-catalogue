#include "stat_reader.h"

namespace transport_catalogue {
    namespace detail {

        namespace stop {

            void PrintQueryStop(TransportCatalogue& catalogue, std::string_view stop_name) {
                auto entry_word = 5;
                stop_name = stop_name.substr(entry_word);
                std::unordered_set<const Bus*> unique_buses;
                std::unordered_set<std::string_view> unique_buses_name;
                std::vector <std::string> bus_name_;

                Stop* stop = catalogue.GetStop(stop_name);

                if (stop != nullptr) {
                    unique_buses = catalogue.StopGetUniqueBuses(stop);

                    if (unique_buses.size() == 0) {
                        std::cout << "Stop " << stop_name << ": no buses" << std::endl;
                    }
                    else {
                        std::cout << "Stop " << stop_name << ": buses ";

                        for (const Bus* _bus : unique_buses) {
                            bus_name_.push_back(_bus->bus_name);
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

            void PrintQueryBus(TransportCatalogue& catalogue, std::string_view str) {
                auto entry_word = 4;
                str = str.substr(entry_word);

                Bus* bus = catalogue.GetBus(str);
                if (bus != nullptr) {
                    std::cout << "Bus " << bus->bus_name << ": "
                        << bus->stops_on_route.size() << " stops on route, "
                        << bus->count_unique_stops << " unique stops, "
                        << catalogue.GetDistanceForBus(bus) << " route length, "
                        << std::setprecision(6) << double(catalogue.GetDistanceForBus(bus)

                            / catalogue.GetLength(bus))
                        << " curvature" << std::endl;
                }
                else {
                    std::cout << "Bus " << str << ": not found" << std::endl;
                }
            }
        }

        void PrintParseQuery(TransportCatalogue& catalogue, std::string_view str) {
            if (str.substr(0, 3) == "Bus") {
                bus::PrintQueryBus(catalogue, str);
            }
            else if (str.substr(0, 4) == "Stop") {
                stop::PrintQueryStop(catalogue, str);
            }
            else {
                std::cout << "Error query" << std::endl;
            }
        }

        void ReadOutputQuery(std::istream& input, TransportCatalogue& catalogue) {
            std::string count;
            std::getline(input, count);

            std::string str;

            auto num = stoi(count);

            for (int i = 0; i < num; ++i) {
                std::getline(input, str);
                PrintParseQuery(catalogue, str);
            }

        }
    }
}