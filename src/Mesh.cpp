#include "../include/Mesh.hpp"

Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, Vertex centralVertex, Material mat){
    this->vertices = vertices;
    this->mat = mat;
    this->centralVertex = centralVertex;
    this->isLight = !(mat.Ke == color3d(0,0,0));
    for(unsigned int i=0;i<indices.size();i+=3){
        point3d a = vertices[indices[i]].pos;
        point3d b = vertices[indices[i+1]].pos;
        point3d c = vertices[indices[i+2]].pos;
        vec3d centroid = (a + b + c) / 3;
        vec3d normal = unit_vec(cross(b-a, c-a));
        faces.push_back(Face(indices[i], indices[i+1], indices[i+2], normal, centroid));
    }
}

double Mesh::intersectTri(Ray r, Face f){
    //Moller Trumbore Algorithm
    point3d a = this->vertices[f.v1].pos;
    point3d b = this->vertices[f.v2].pos;
    point3d c = this->vertices[f.v3].pos;
    
    vec3d edge1 = b - a;
    vec3d edge2 = c - a;
    vec3d pVec = cross(r.direction, edge2);
    double M = dot(pVec, edge1);

    //Rejecting if the the ray is parallel to the triangle
    if(checkEquality(M, 0.0f)){
        return -1;
    }

    double invCosine = 1.0f / M;
    vec3d tVec = r.origin - a;
    double u = invCosine * dot(pVec, tVec);
    vec3d qVec = cross(tVec, edge1);
    double v = invCosine * dot(qVec, r.direction);

    //Rejecting if the intersection point is outside the triangle
    if(u < 0.0f || u > 1.0f || v < 0.0f || u + v > 1.0f){
        return -1;
    }

    double t = invCosine * dot(qVec, edge2);
    return t;
}

double Mesh::triArea(point3d a, point3d b, point3d c){
    vec3d v1 = b - a;
    vec3d v2 = c - a;
    vec3d v = cross(v1, v2);
    return 0.5 * v.length();
}

void Mesh::intersect(Ray r, double t_min, double t_max, int mesh_index, hit_record &rec){
    for(unsigned int i=0;i<faces.size();i++){
        double t = intersectTri(r,faces[i]);
        if(t>0){
            if(t > t_min && t < t_max){
                t_max = t;
                rec.t = t;
                rec.pt = r.origin + t * r.direction;
                vec3d n = faces[i].normal;
                double k = dot(r.direction,n);
                rec.normal = (k<0)?n:-n;
                rec.tangent = unit_vec(vertices[faces[i].v2].pos - vertices[faces[i].v1].pos);
                rec.mat = this->mat;
                rec.face_index = i;
                rec.mesh_index = mesh_index;
                rec.isHit = true;
            }
        }
    }
}