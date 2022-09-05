#include "../include/Scene.hpp"
#include "../include/Sampler.hpp"

Scene::Scene(string obj_path, string config_path){
    loadScene(obj_path);
    loadCamera(config_path);
}

void Scene::loadScene(string path){
    Assimp::Importer importer;
    //Require explicit triangulation since the object file may contain polygonal faces in general 
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
    Vertex centralVertex; 
    int N = mesh->mNumVertices;

    //Process Vertices
    for(unsigned int i=0;i<mesh->mNumVertices;i++){
        point3d pos(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        // vec3d normal(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        //normals read by assimp are not necessarily normalized
        centralVertex.pos += pos / N; 
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

    if(!(mat.Ke == color3d(0.0f))){
        lights.push_back(this->meshes.size());
    }

    return Mesh(vertices, indices, centralVertex, mat);
}


//Ray Casting
void Scene::rayCast(vector<vector<color3d>>& pixelMap, int samples, int n_threads){
    int width = cam->getWidth();
    int height = cam->getHeight();
    int window_length = (height + n_threads - 1) / n_threads;

    //Initializing pixelMap
    pixelMap = vector<vector<color3d>>(height,vector<color3d>(width,color3d()));
    #pragma omp parallel num_threads(n_threads)
    {
        int thread_id = omp_get_thread_num();
        int x_high = min((thread_id + 1) * window_length, height);
        int x_low = thread_id * window_length;
        for(int x=x_low;x<x_high;x++){
            for(int y=0;y<width;y++){
                color3d new_color(0.0f);
                for(int i = 0;i < samples; i++){
                    double u = x - 0.5 + random_double();
                    double v = y - 0.5 + random_double();
                    Ray ray = cam->getRay(v, u);
                    new_color += rayTrace(ray, 0);
                }
                pixelMap[x][y] = new_color / samples;
            }
        }
    }
}

//Ray Tracing
color3d Scene::rayTrace(Ray ray, int depth){
    if(depth > MAX_DEPTH){
        return BG_COLOR;
    }else{
        //Obtain the intersection point of the ray with the current geometry 
        hit_record rec = intersect(ray);

        //Light Transport Algorithm 
        if(rec.isHit){
            //Outgoing radiance
            color3d Lo = color3d(0.0f);

            //Emitted radiance 
            color3d Le = rec.mat.Ke * color3d(255.0f);
            Lo += Le;

            //Tracing incoming radiance recursively
            //Steps to do:
            //1) Sample incoming rays 
            //2) Recursively trace those rays for computing their radiance
            //3) Compute the contribution of that ray by multiplying the computed radiance
            // with the BRDF separately for all components of the radiance
            //4) Add the weighted contributions of the sampled rays to compute the total
            // outgoing radiance 
            vec3d pt = rec.pt;
            vec3d n = rec.normal;
            vec3d t = rec.tangent;
            vec3d wo = -ray.direction; //Direction of outgoing ray
            //BRDF values for the three components
            color3d ka = rec.mat.Ka; 
            color3d kd = rec.mat.Kd;
            color3d ks = rec.mat.Ks;

            //Sampling incoming rays
            int N = 3; //Number of samples
            color3d Li(0.0f);
            for(int i=0;i<N;i++){
                
            }
            Li = Li / N;
            Lo += Li;

            return Lo;
        }else{
            return BG_COLOR;
        }
    }
}

//Processing intersections(without acceleraation)
hit_record Scene::intersect(Ray r){
    hit_record rec; 
    //Not zero for handling numerical errors
    double t_min = 0.001;
    for(unsigned int i = 0; i < this->meshes.size(); i++){
        double t_max = rec.t;
        this->meshes[i].intersect(r, t_min, t_max, i, rec);
    } 
    return rec; 
}

//Getting light cones from the given point
unordered_map<unsigned int, double> Scene::getLightCones(point3d p){
    unordered_map<unsigned int, double> light_cones; 
    for(unsigned int i = 0; i < this->lights.size(); i++){
        int lightIndex = lights[i];
        double cone_angle_cosine = this->getLightCone(p, lightIndex); 
        light_cones[lightIndex] = cone_angle_cosine;
    }
    return light_cones; 
}

double Scene::getLightCone(point3d p, int index){
    point3d centralVertex = this->meshes[index].centralVertex.pos;
    vec3d centralLightDir = centralVertex - p;
    double cosine = 1.0f;
    for(unsigned int i=0;i<this->meshes[index].vertices.size();i++){
        vec3d lightDir = this->meshes[index].vertices[i].pos - p;
        cosine = min(cosine, dot(centralLightDir, lightDir));
    }
    return cosine;
}
