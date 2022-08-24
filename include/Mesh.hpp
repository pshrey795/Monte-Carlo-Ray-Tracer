#ifndef MESH_HPP
#define MESH_HPP

#include "vec3d.hpp"
#include <bits/stdc++.h>

using namespace std;

struct Vertex{
    point3d pos;
    vec3d normal;
    Vertex(point3d pos, vec3d normal){
        this->pos = pos;
        this->normal = normal;
    }
};

class Mesh {
    public:
        vector<Vertex> vertices;
        vector<unsigned int> faces;
        Mesh(vector<Vertex> vertices, vector<unsigned int> indices);
         
};

#endif