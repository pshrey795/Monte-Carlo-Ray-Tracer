#ifndef OBJECT_HPP
#define OBJECT_HPP


#include "Ray.hpp"
#include <bits/stdc++.h>

#define INF DBL_MAX

using namespace std;

struct Vertex{
    point3d pos;
    Vertex(point3d pos){
        this->pos = pos; 
    }
};

struct Face{
    int v1,v2,v3;
    vec3d normal;
    Face(int v1, int v2, int v3, vec3d normal){
        this->v1 = v1;
        this->v2 = v2;
        this->v3 = v3;
        this->normal = normal; 
    }
};

struct Material{
    color3d Ka;         //ambient
    color3d Kd;         //diffuse
    color3d Ks;         //specular
    color3d Ke;         //emission (light source)

    //Set default constructor
    Material(){
        Ka = color3d(0,0,0);
        Kd = color3d(0,0,0);
        Ks = color3d(0,0,0);
        Ke = color3d(0,0,0);
    }
};

struct hit_record {
    point3d pt;
    vec3d normal;
    Material mat;
    double t;
    int face_index;
    int mesh_index;
    bool isHit;

    hit_record(){
        t = INF;
        isHit = false;
        face_index = -1;
        mesh_index = -1;
    } 
};

class Object3D {

    protected:
        bool checkEquality(double v1,double v2){
            return (abs(v1-v2) <= threshold);
        }

    public:
        Material mat;
        void intersect(Ray r, double lower_limit, double upper_limit, hit_record& rec);

    private:
        const double threshold = 1e-3;

};

#endif