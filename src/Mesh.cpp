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
        vec3d normal = unit_vec(cross(b-a, c-a));
        faces.push_back(Face(indices[i], indices[i+1], indices[i+2], normal));
    }
}

double Mesh::intersectPlane(Ray r, Plane p){
    double k = dot(r.direction,p.n);
    if(k==0){
        return -1;
    }else{
        double t = dot(p.pt - r.origin,p.n) / k;
        return t;
    }
}

double Mesh::intersectTri(Ray r, Face f){
    point3d a = this->vertices[f.v1].pos;
    point3d b = this->vertices[f.v2].pos;
    point3d c = this->vertices[f.v3].pos;
    Plane p(a,unit_vec(cross(b - a, c - a)));
    double t = intersectPlane(r,p);
    point3d pt = r.origin + t * r.direction;
    double A = this->triArea(a,b,c);
    double A1 = this->triArea(pt,b,c);
    double A2 = this->triArea(pt,a,c);
    double A3 = this->triArea(pt,a,b);
    bool isInside = (abs((A1 + A2 + A3) - A) < DELTA);
    if(isInside){
        return t;
    }else{
        return -1;
    }
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
                rec.normal = faces[i].normal;
                rec.tangent = unit_vec(vertices[faces[i].v2].pos - vertices[faces[i].v1].pos);
                rec.mat = this->mat;
                rec.face_index = i;
                rec.mesh_index = mesh_index;
                rec.isHit = true;
            }
        }
    }
}