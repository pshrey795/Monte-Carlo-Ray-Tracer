#ifndef MESH_HPP
#define MESH_HPP

#include "Object3D.hpp"
#define DELTA 0.01

struct Plane {
    point3d pt;
    vec3d n;
    Plane(point3d pt, vec3d n){
        this->pt = pt;
        this->n = n;
    }
};

class Mesh : public Object3D {
    public:
        vector<Vertex> vertices;
        vector<Face> faces;
        Mesh(vector<Vertex> vertices, vector<unsigned int> indices, Material mat);
        void intersect(Ray r, double t_min, double t_max, int mesh_index, hit_record &rec);

    private:
        double intersectPlane(Ray r, Plane p);
        double intersectTri(Ray r, Face f);
        double triArea(point3d a, point3d b, point3d c);
};

#endif