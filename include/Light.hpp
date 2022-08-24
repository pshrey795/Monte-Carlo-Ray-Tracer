#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "vec3d.hpp"

class Light {
    public: 
        Light(point3d pos, color3d color){
            this->light_color = color;
            this->light_pos = pos; 
        }
        point3d light_pos; 
        color3d light_color; 

};

#endif