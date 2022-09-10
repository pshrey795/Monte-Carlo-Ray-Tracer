#include "../include/BVH.hpp"

double min(double a, double b){
    return a < b ? a : b;
}

double max(double a, double b){
    return a > b ? a : b;
}

bool intersectBox(Ray r, double t_min, double t_max, point3d box_min, point3d box_max){
    //Slab Test
    //X direction 
    if(r.direction[0] != 0){
        double t0_x = (box_min[0] - r.origin[0]) * r.inv_direction[0];
        double t1_x = (box_max[0] - r.origin[0]) * r.inv_direction[0];
        t_min = max(t_min, min(t0_x, t1_x));
        t_max = min(t_max, max(t0_x, t1_x));
    }
    //Y direction
    if(r.direction[1] != 0){
        double t0_y = (box_min[1] - r.origin[1]) * r.inv_direction[1];
        double t1_y = (box_max[1] - r.origin[1]) * r.inv_direction[1];
        t_min = max(t_min, min(t0_y, t1_y));
        t_max = min(t_max, max(t0_y, t1_y));
    }
    //Z direction 
    if(r.direction[2] != 0){
        double t0_z = (box_min[2] - r.origin[2]) * r.inv_direction[2];
        double t1_z = (box_max[2] - r.origin[2]) * r.inv_direction[2];
        t_min = max(t_min, min(t0_z, t1_z));
        t_max = min(t_max, max(t0_z, t1_z));
    }
    return t_max >= t_min;
}

double intersectBoxValue(Ray r, double t_min, double t_max, point3d box_min, point3d box_max){
    if(r.direction[0] != 0){
        double t0_x = (box_min[0] - r.origin[0]) * r.inv_direction[0];
        double t1_x = (box_max[0] - r.origin[0]) * r.inv_direction[0];
        t_min = max(t_min, min(t0_x, t1_x));
        t_max = min(t_max, max(t0_x, t1_x));
    }
    //Y direction
    if(r.direction[1] != 0){
        double t0_y = (box_min[1] - r.origin[1]) * r.inv_direction[1];
        double t1_y = (box_max[1] - r.origin[1]) * r.inv_direction[1];
        t_min = max(t_min, min(t0_y, t1_y));
        t_max = min(t_max, max(t0_y, t1_y));
    }
    //Z direction 
    if(r.direction[2] != 0){
        double t0_z = (box_min[2] - r.origin[2]) * r.inv_direction[2];
        double t1_z = (box_max[2] - r.origin[2]) * r.inv_direction[2];
        t_min = max(t_min, min(t0_z, t1_z));
        t_max = min(t_max, max(t0_z, t1_z));
    }
    if(t_max >= t_min){
        return t_min;
    }else{
        return -1;
    }
}

