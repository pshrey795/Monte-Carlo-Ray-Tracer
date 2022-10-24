#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "Ray.hpp"

class Camera {
    public:
        Camera();
        Camera(point3d eye, point3d lookAt, vec3d up_dir, int screen_width, int screen_height, int screen_depth, float factor);
        Ray getRay(double x, double y);
        int getWidth();
        int getHeight();

    private:
        point3d eye;
        point3d lookAt;
        vec3d view_dir, up_dir, right_dir;
        int screen_width, screen_height, screen_depth;
        float factor; //factor to scale the screen
        point3d origin; 

};

#endif