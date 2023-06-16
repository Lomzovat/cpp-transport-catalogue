#pragma once

#include <cmath>

namespace transport_catalogue {
    namespace detail {
        namespace geo {


            struct Coordinates {
                double latitude;
                double longitude;

                bool operator==(const Coordinates& other) const {
                    return latitude == other.latitude
                        && longitude == other.longitude;
                }
            };

            const double PI = 3.1415926535;
            const int EARTH_R = 6371000;


            inline double ComputeDistance(Coordinates start, Coordinates end) {
                using namespace std;
                if (!(start == end)) {
                    const double dr = PI / 180.;
                    return acos(sin(start.latitude * dr) * sin(end.latitude * dr)
                        + cos(start.latitude * dr) * cos(end.latitude * dr)
                        * cos(abs(start.longitude - end.longitude) * dr)) * EARTH_R;
                }
                else {
                    return 0.0;
                }
            }
