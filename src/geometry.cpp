#include <geometry.h>
vec2 vec3::toSpherical() {
    double theta = atan2(sqrt(x*x + y*y), z);
    double phi = atan2(y, x);
    return vec2{theta, phi};
}

vec3 toCartesian(vec2 spherical) {
    spherical = degToRad(spherical);
    return vec3{
        sin(spherical.y) * cos(spherical.x),
        sin(spherical.y) * sin(spherical.x),
        cos(spherical.y)
    };
}

vec2 toSpherical(vec3 cartesian) {
    double r = std::sqrt(cartesian.x * cartesian.x + cartesian.y * cartesian.y + cartesian.z * cartesian.z);       // Radius
    double theta = std::atan2(cartesian.y, cartesian.x);
    double phi = std::acos(cartesian.z / r);
    return radToDeg(vec2{theta, phi});
}