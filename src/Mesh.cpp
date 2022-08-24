#include "../include/Mesh.hpp"

Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices){
    this->vertices = vertices;
    this->faces = indices;
}