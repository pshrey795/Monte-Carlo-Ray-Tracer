#ifndef SCENE_HPP
#define SCENE_HPP

#include "Mesh.hpp" 
#include "Camera.hpp"
#include "BVH.hpp"
#include<omp.h>

#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>

#define MAX_DEPTH 1
#define BG_COLOR color3d(0.5*255,0.7*255,0.9*255);

using namespace std;

class Scene {
    public:
        Scene(string obj_path, string config_path);

        //Casting rays for starting the ray tracing
        void castRays(vector<vector<color3d>>& pixelMap, int samples, int n_threads);

    private:
        Camera* cam; 
        vector<Mesh> meshes;
        vector<unsigned int> lights; //Indices of lights in meshes
        void loadScene(string path);
        void loadCamera(string path);
        void processNode(aiNode* node, const aiScene* scene);
        Mesh processMesh(aiMesh* mesh, const aiScene* scene);

        //Bounding Volume Hierarchy 
        BVH bvh; 

        //Getting light cones from the given point
        double getLightCone(point3d p, int index);
        unordered_map<unsigned int, double> getLightCones(point3d p, int curr_index);

        //Ray Tracing
        color3d traceRay(Ray ray, int depth);
        hit_record intersect(Ray r);   

        //Computing Illuminations 
        color3d computeLocalIllumination(Ray wo, hit_record rec, int depth, int samples_per_source);
        color3d computeGlobalIllumination(Ray wo, hit_record rec, int depth, int samples);     
};

#endif