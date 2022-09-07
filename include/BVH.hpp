#ifndef BVH_HPP
#define BVH_HPP

#include "Ray.hpp"
#define MIN_PRIM_COUNT 2

//Bounds of a BVH node depending upon the type of the bounding box used 
//AABB as of now 
struct nodeBounds {
    point3d nodeMin;
    point3d nodeMax;
};

//Structure of a normal BVH Node
struct BVHNode {
    nodeBounds bounds;
    int leftChild, rightChild;
    int firstPrim, primCount;
    BVHNode(){
        bounds.nodeMax = point3d(DBL_MIN);
        bounds.nodeMin = point3d(DBL_MAX);
        leftChild = rightChild = 0;
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

#endif