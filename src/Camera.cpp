#include "../include/Camera.hpp"

using namespace std;

Camera::Camera(point3d eye, point3d lookAt, vec3d up_dir, int screen_width, int screen_height, int screen_depth){
    this->eye = eye;
    this->lookAt = lookAt;
    this->view_dir = unit_vec(lookAt - eye);
    this->up_dir = unit_vec(up_dir);
    this->right_dir = unit_vec(cross(view_dir,up_dir));
    this->origin = eye + screen_depth * this->view_dir - (screen_height/2) * this->up_dir - (screen_width/2) * this->right_dir;
}

Ray Camera::getRay(double x, double y){
    point3d rayOrigin = this->eye;
    vec3d rayDir = this->origin + x * this->right_dir + y * this->up_dir - this->eye;
    return Ray(rayOrigin,rayDir);
}