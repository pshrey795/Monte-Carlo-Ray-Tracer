#include "../include/Scene.hpp"

Scene::Scene(string obj_path, string config_path){
    loadScene(obj_path);
    loadCamera(config_path);
}

void Scene::loadScene(string path){
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
        cout << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
        return;
    }
    processNode(scene->mRootNode, scene);
}

void Scene::loadCamera(string path){
    ifstream myFile(path);
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
    getline(myFile, line);
    screen_height = stoi(line);
    getline(myFile, line);
    screen_depth = stoi(line);

    //Creating camera
    this->cam = new Camera(camPos, lookAt, camUp, screen_width, screen_height, screen_depth);
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
    vector<unsigned int> indices;

    //Process Vertices
    for(unsigned int i=0;i<mesh->mNumVertices;i++){
        point3d pos(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        // vec3d normal(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        //normals read by assimp are not necessarily normalized
        vertices.push_back(Vertex(pos));
    }

    //Process Faces
    for(unsigned int i=0;i<mesh->mNumFaces;i++){
        aiFace face = mesh->mFaces[i];
        for(unsigned int j=0;j<face.mNumIndices;j++){
            indices.push_back(face.mIndices[j]);
        }
    }

    //Process Material
    Material mat;
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    aiColor3D color(0.0f, 0.0f, 0.0f);
    material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
    mat.Kd = color3d(color.r, color.g, color.b);
    material->Get(AI_MATKEY_COLOR_SPECULAR, color);
    mat.Ks = color3d(color.r, color.g, color.b);
    material->Get(AI_MATKEY_COLOR_AMBIENT, color);
    mat.Ka = color3d(color.r, color.g, color.b);
    material->Get(AI_MATKEY_COLOR_EMISSIVE, color);
    mat.Ke = color3d(color.r, color.g, color.b);

    return Mesh(vertices, indices, mat);
}


//Ray Casting
void Scene::rayCast(vector<vector<color3d>>& pixelMap){
    int width = cam->getWidth();
    int height = cam->getHeight();

    //Initializing pixelMap
    pixelMap = vector<vector<color3d>>(height,vector<color3d>(width,color3d()));
    for(int i=0;i<height;i++){
        for(int j=0;j<width;j++){
            Ray ray = cam->getRay(j, i);
            pixelMap[i][j] = rayTrace(ray, 0);
        }
    }
}

//Ray Tracing
color3d Scene::rayTrace(Ray ray, int depth){
    if(depth > MAX_DEPTH){
        return BG_COLOR;
    }else{
        //Obtain the intersection point of the ray with the current geometry 


        //Invoke the light transport algorithm at the current point recursively 


        return BG_COLOR;
    }
}