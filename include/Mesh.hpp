#ifndef MESH_HPP
#define MESH_HPP

#include "Object3D.hpp"

class Mesh : public Object3D {
    public:
        vector<Vertex> vertices;
        vector<Face> faces;

        Mesh(vector<Vertex> vertices, vector<unsigned int> indices, Material mat);
         
};

#endif