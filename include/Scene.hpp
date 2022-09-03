#ifndef SCENE_HPP
#define SCENE_HPP

#include "Mesh.hpp" 
#include "Camera.hpp"

#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>

#define MAX_DEPTH 5
#define BG_COLOR color3d(0.5*255,0.7*255,0.9*255);

using namespace std;

class Scene {
    public:
        Scene(string obj_path, string config_path);

        //Casting rays for starting the ray tracing
        void rayCast(vector<vector<color3d>>& pixelMap);

    private:
        Camera* cam; 
        vector<Mesh> meshes;
        void loadScene(string path);
        void loadCamera(string path);
        void processNode(aiNode* node, const aiScene* scene);
        Mesh processMesh(aiMesh* mesh, const aiScene* scene);

        //Ray Tracing
        color3d rayTrace(Ray ray, int depth);
        void intersect(Ray r, double lower_limit, double upper_limit, hit_record& rec);        
};

#endif