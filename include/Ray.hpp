#ifndef RAY_HPP
#define RAY_HPP

#include "vec3d.hpp"

class Ray {
    public:
        Ray(point3d origin, vec3d direction){
            this->origin = origin;
            this->direction = unit_vec(direction);
            this->inv_direction = vec3d(1.0f/direction[0], 1.0f/direction[1], 1.0f/direction[2]);
        }
        point3d origin;
        vec3d direction;
        vec3d inv_direction;
};

#endif