#include "../include/Mesh.hpp"

Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, Material mat){
    this->vertices = vertices;
    this->mat = mat;
    for(unsigned int i=0;i<indices.size();i+=3){
        faces.push_back(Face(indices[i], indices[i+1], indices[i+2]));
    }
}