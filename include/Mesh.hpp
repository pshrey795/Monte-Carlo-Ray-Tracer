#ifndef MESH_HPP
#define MESH_HPP

#include "Object3D.hpp"
#include "BVH.hpp"
#define DELTA 0.01

struct Vertex{
    point3d pos;
    Vertex(point3d pos){
        this->pos = pos; 
    }
    Vertex(){
        this->pos = point3d(0.0f);
    }
};

struct Face{
    int v1,v2,v3;
    vec3d normal;
    vec3d centroid;
    Face(int v1, int v2, int v3, vec3d normal, vec3d centroid){
        this->v1 = v1;
        this->v2 = v2;
        this->v3 = v3;
        this->normal = normal; 
        this->centroid = centroid;
    }
};

class Mesh : public Object3D {
    public:
        vector<Vertex> vertices;
        vector<Face> faces;
        vector<BVHNode> nodeList;   
        bool isLight = false;
        Vertex centralVertex;  
        Mesh(vector<Vertex> vertices, vector<unsigned int> indices, Vertex centralVertex, Material mat);
        void intersectBVH(Ray r, double t_min, double t_max, int mesh_index, hit_record &rec, int nodeIndex);

    private:
        double intersectTri(Ray r, Face f);
        double triArea(point3d a, point3d b, point3d c);

        //BVH Construction routines
        vector<int> primitives; 
        int currentNodeIndex = 0; 
        int numNodes = 0;
        void constructBVH();
        void updateBounds(int nodeIndex);
        void generateNodes(int nodeIndex);

        //Surface Area Heuristic 
        double surfaceAreaMeasure(BVHNode& node, int axis, double currPos);

};

#endif