#ifndef SCENE_HPP
#define SCENE_HPP

#include "Mesh.hpp" 
#include "Camera.hpp"
#include "BVH.hpp"
#include<omp.h>

#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>

using namespace std;

class Scene {
    public:
        Scene(string obj_path, string config_path, int direct_samples, int indirect_samples, int max_depth, int min_primitives);

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
        BVHRootNode bvhRoot; 

        //Getting light cones from the given point
        double getLightCone(point3d p, int index);
        unordered_map<unsigned int, double> getLightCones(point3d p, int curr_index);

        //Ray Tracing
        color3d traceRay(Ray ray, int depth);
        hit_record intersect(Ray r);   

        //Computing Illuminations 
        color3d traceDiffuseRay(hit_record rec, int depth);
        color3d computeLocalIllumination(hit_record rec, int depth, int samples_per_source);
        color3d computeGlobalIllumination(hit_record rec, int depth, int samples); 
        color3d traceSpecularRay(Ray wo, hit_record rec, int depth);

        //Parameters for sampling
        int direct_samples_per_ray;
        int indirect_samples_per_ray;
        int tracing_depth;
        int bvh_min_primitives;
    
};

#endif