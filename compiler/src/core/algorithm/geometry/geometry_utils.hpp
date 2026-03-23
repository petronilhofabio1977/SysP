#pragma once
#include <cmath>

namespace sysp::core::algorithm {

struct Point2D { double x, y; };
struct Point3D { double x, y, z; };

inline double distance2D(Point2D a, Point2D b) {
    double dx = a.x - b.x, dy = a.y - b.y;
    return std::sqrt(dx*dx + dy*dy);
}

inline double cross2D(Point2D o, Point2D a, Point2D b) {
    return (a.x - o.x) * (b.y - o.y) - (a.y - o.y) * (b.x - o.x);
}

inline double dot3D(Point3D a, Point3D b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

inline Point3D cross3D(Point3D a, Point3D b) {
    return {
        a.y*b.z - a.z*b.y,
        a.z*b.x - a.x*b.z,
        a.x*b.y - a.y*b.x
    };
}

} // namespace sysp::core::algorithm
