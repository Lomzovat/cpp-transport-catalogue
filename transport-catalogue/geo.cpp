#define _USE_MATH_DEFINES
#include "geo.h"


namespace geo {


    double ComputeDistance(Coordinates from, Coordinates to) {
        using namespace std;
        if (from == to) {
            return 0;
        }
        static const double dr = M_PI / 180.;
        return acos(sin(from.latitude * dr) * sin(to.latitude * dr)
            + cos(from.latitude * dr) * cos(to.latitude * dr) * cos(abs(from.longitude - to.longitude) * dr))
            * EARTH_R;
    }

}  // namespace geo