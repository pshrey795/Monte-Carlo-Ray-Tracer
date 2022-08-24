#ifndef SCENE_HPP
#define SCENE_HPP

#include "Mesh.hpp" 
#include "Light.hpp"
#include "Camera.hpp"

#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>

using namespace std;

class Scene {
    public:
        Scene(string obj_path, string config_path, int& width, int& height);

    private:
        Camera* cam; 
        vector<Light> lights;
        vector<Mesh> meshes;
        string directory;
        void loadScene(string path);
        void loadCamera(string path, int& width, int& height);
        void processLights(const aiScene* scene);
        void processNode(aiNode* node, const aiScene* scene);
        Mesh processMesh(aiMesh* mesh, const aiScene* scene);
};

#endif