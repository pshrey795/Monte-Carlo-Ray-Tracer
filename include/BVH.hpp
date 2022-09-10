#ifndef BVH_HPP
#define BVH_HPP

#include "Ray.hpp"

//Bounds of a BVH node depending upon the type of the bounding box used 
//AABB as of now 
struct nodeBounds {
    point3d nodeMin;
    point3d nodeMax;
    void expand(point3d p){
        nodeMin = min(nodeMin, p);
        nodeMax = max(nodeMax, p);
    }
    double surfaceArea(){
        vec3d diag = nodeMax - nodeMin;
        return 2 * (diag[0] * diag[1] + diag[0] * diag[2] + diag[1] * diag[2]);
    }
};

//Structure of a normal BVH Node
struct BVHNode {
    nodeBounds bounds;
    int firstPrim, primCount;
    BVHNode(){
        bounds.nodeMax = point3d(DBL_MIN);
        bounds.nodeMin = point3d(DBL_MAX);
        firstPrim = primCount = 0;
    }
};

//Structure of a BVH root node 
struct BVHRootNode {
    nodeBounds bounds;
    BVHRootNode(){
        bounds.nodeMax = point3d(DBL_MIN);
        bounds.nodeMin = point3d(DBL_MAX);
    }
};

bool intersectBox(Ray r, double t_min, double t_max, point3d min, point3d max);
double intersectBoxValue(Ray r, double t_min, double t_max, point3d min, point3d max);

#endif