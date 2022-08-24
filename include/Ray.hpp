#ifndef RAY_HPP
#define RAY_HPP

#include "vec3d.hpp"

class Ray {
    public:
        Ray(point3d origin, vec3d direction){
            this->origin = origin;
            this->direction = unit_vec(direction);
        }
        point3d origin;
        vec3d direction;
};

#endif