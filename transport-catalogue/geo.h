#pragma once

#include <cmath>


namespace geo {

    const double PI = 3.1415926535;
    const int EARTH_R = 6371000;


    struct Coordinates {
        double latitude;
        double longitude;
        bool operator==(const Coordinates& other) const {
            return latitude == other.latitude && longitude == other.longitude;
        }
        bool operator!=(const Coordinates& other) const {
            return !(*this == other);
        }
    };

    inline double ComputeDistance(Coordinates from, Coordinates to) {
        using namespace std;
        if (from == to) {
            return 0;
        }
        static const double dr = PI / 180.;
        return acos(sin(from.latitude * dr) * sin(to.latitude * dr)
            + cos(from.latitude * dr) * cos(to.latitude * dr) * cos(abs(from.longitude - to.longitude) * dr))
            * EARTH_R;
    }
}
