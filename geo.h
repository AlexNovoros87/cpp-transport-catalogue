#pragma once
namespace geo {
    const double PI = 3.1415926535;
    const double HALF_CIRCLE_DEG = 180.;
    const int EARTH_RADIUS = 6371000;

    struct Coordinates {
        double lat;
        double lng;
        bool operator==(const Coordinates& other) const;
        bool operator!=(const Coordinates& other) const;
    };

    double ComputeDistance(Coordinates from, Coordinates to);
}