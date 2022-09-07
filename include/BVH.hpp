#ifndef BVH_HPP
#define BVH_HPP

#include "Mesh.hpp"

//Bounds of a BVH node depending upon the type of the bounding box used 
//AABB as of now 
struct nodeBounds {
    point3d nodeMin;
    point3d nodeMax;
};


//Structure of a single BVH Node
struct BVHNode {
    BVHNode* left;
    BVHNode* right;
    nodeBounds bounds;
    vector<unsigned int> primitives;
};

class BVH {
    public:
        BVH();

    private:
        BVHNode* rootNode; 


};

#endif