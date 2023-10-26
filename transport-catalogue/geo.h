#pragma once

#include <cmath>

namespace geo {

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

    double ComputeDistance(Coordinates from, Coordinates to);

}  // namespace geo
