#include "../include/Scene.hpp"

Scene::Scene(string obj_path, string config_path, int& width, int& height){
    loadScene(obj_path);
    loadCamera(config_path, width, height);
}

void Scene::loadScene(string path){
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
        cout << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));
    processLights(scene);
    processNode(scene->mRootNode, scene);
    processLights(scene); 
}

void Scene::loadCamera(string path, int& width, int& height){
    ifstream myFile;
    myFile.open(path);
    string line;

    int screen_width, screen_height, screen_depth;
    point3d camPos, lookAt;
    vec3d camUp; 

    //Skipping irrelevant lines
    getline(myFile, line);
    while(line.substr(0,line.length()-1) != "camera"){
        getline(myFile, line);
    }

    //Obtaining camera parameters
    getline(myFile, line);
    camPos = readVec(line);
    getline(myFile, line);
    lookAt = readVec(line);
    getline(myFile, line);
    camUp = readVec(line);
    getline(myFile, line);
    screen_width = stoi(line);
    width = screen_width;
    getline(myFile, line);
    screen_height = stoi(line);
    height = screen_height;
    getline(myFile, line);
    screen_depth = stoi(line);

    //Creating camera
    this->cam = new Camera(camPos, lookAt, camUp, screen_width, screen_height, screen_depth);
}

void Scene::processLights(const aiScene* scene){
    for(unsigned int i = 0; i < scene->mNumLights; i++){
        point3d pos(scene->mLights[i]->mPosition.x, scene->mLights[i]->mPosition.y, scene->mLights[i]->mPosition.z);
        color3d col(scene->mLights[i]->mColorDiffuse.r, scene->mLights[i]->mColorDiffuse.g, scene->mLights[i]->mColorDiffuse.b);
        lights.push_back(Light(pos,col));
    }
}

void Scene::processNode(aiNode* node, const aiScene* scene){
    for(unsigned int i = 0; i < node->mNumMeshes; i++){
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    for(unsigned int i = 0; i < node->mNumChildren; i++){
        processNode(node->mChildren[i], scene);
    }
}

Mesh Scene::processMesh(aiMesh* mesh, const aiScene* scene){
    vector<Vertex> vertices; 
    vector<unsigned int> faces;

    //Process Vertices
    for(int i=0;i<mesh->mNumVertices;i++){
        point3d pos(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        vec3d normal(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        vertices.push_back(Vertex(pos, normal));
    }

    //Process Faces
    for(int i=0;i<mesh->mNumFaces;i++){
        aiFace face = mesh->mFaces[i];
        for(int j=0;j<face.mNumIndices;j++){
            faces.push_back(face.mIndices[j]);
        }
    }

    //TODO: Process Material

    return Mesh(vertices, faces);
}