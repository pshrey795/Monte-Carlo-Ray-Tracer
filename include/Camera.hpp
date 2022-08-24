#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "Ray.hpp"

class Camera {
    public:
        Camera();
        Camera(point3d eye, point3d lookAt, vec3d up_dir, int screen_width, int screen_height, int screen_depth);
        Ray getRay(double x, double y);

    private:
        point3d eye;
        point3d lookAt;
        vec3d view_dir, up_dir, right_dir;
        point3d origin; 

};

#endif