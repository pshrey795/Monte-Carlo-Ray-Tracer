#include "../include/Scene.hpp"
#include "../include/Sampler.hpp"

Scene::Scene(string obj_path, string config_path, int direct_samples, int indirect_samples, int max_depth, int min_primitives){
    loadScene(obj_path);
    loadCamera(config_path);
    this->direct_samples_per_ray = direct_samples;
    this->indirect_samples_per_ray = indirect_samples;
    this->tracing_depth = max_depth;
    this->bvh_min_primitives = min_primitives;
}

void Scene::loadScene(string path){
    Assimp::Importer importer;
    //Require explicit triangulation since the object file may contain polygonal faces in general 
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
        cout << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
        return;
    }
    bvhRoot = BVHRootNode();
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
    myFile.close();
}

void Scene::processNode(aiNode* node, const aiScene* scene){
    for(unsigned int i = 0; i < node->mNumMeshes; i++){
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        Mesh newMesh = processMesh(mesh, scene);
        bvhRoot.bounds.nodeMin = min(bvhRoot.bounds.nodeMin, newMesh.nodeList[0].first.bounds.nodeMin);
        bvhRoot.bounds.nodeMax = max(bvhRoot.bounds.nodeMax, newMesh.nodeList[0].first.bounds.nodeMax);
        meshes.push_back(newMesh);
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
    float specularity;
    material->Get(AI_MATKEY_SHININESS, specularity);
    mat.n = specularity;

    if(!(mat.Ke == color3d(0.0f))){
        lights.push_back(this->meshes.size());
    }

    return Mesh(vertices, indices, centralVertex, mat, bvh_min_primitives);
}

//Ray Casting
void Scene::castRays(vector<vector<color3d>>& pixelMap, int samples, int n_threads){
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
                    new_color += traceRay(ray, 0);
                }
                pixelMap[x][y] = new_color;
            }
        }
    }
}

//Ray Tracing
color3d Scene::traceRay(Ray ray, int depth){
    if(depth > tracing_depth){
        return color3d(0.0f);
    }else{
        //Obtain the intersection point of the ray with the current geometry 
        hit_record rec = intersect(ray);

        //Light Transport Algorithm 
        if(rec.isHit){
            if(!(rec.mat.Ke == color3d(0.0f))){
                //Current object is a light source 
                if(depth == 0){
                    return dot(rec.normal, -ray.direction) * color3d(1.0f) + (rec.mat.Ke * dot(rec.normal, -ray.direction) * color3d(1.0f));
                }else{
                    return (rec.mat.Ke * dot(rec.normal, -ray.direction) * color3d(1.0f));
                }
            }else{
                //Incoming radiance using Monte Carlo Integration 
                //Break total incoming radiation into two parts: 
                //Local + Global 

                //Outgoing radiance
                color3d Lo = color3d(0.0f);
                
                //Tracing Diffuse and Ambient Rays
                color3d diffuseValue = (traceDiffuseRay(rec, depth));
                Lo = Lo + diffuseValue;

                //Tracing Specular Rays
                color3d specularValue = traceSpecularRay(ray, rec, depth);
                Lo = Lo + specularValue;

                return Lo;
            }
        }else{
            return color3d(0.0f);
        }
    }
}

//Tracing Secondary Rays 
color3d Scene::traceDiffuseRay(hit_record rec, int depth){
    color3d Ld(0.0f);

    //Local(Due to light sources)
    Ld = Ld + (computeLocalIllumination(rec, depth, direct_samples_per_ray) * 0.8);

    //Global(Due to indirect lighting from other objects)
    Ld = Ld + (computeGlobalIllumination(rec, depth, indirect_samples_per_ray) * 0.2);

    return Ld;
}

color3d Scene::traceSpecularRay(Ray wo, hit_record rec, int depth){
    vec3d wi = reflect(wo.direction, rec.normal);
    Ray secondaryRay(rec.pt, wi);
    return (rec.mat.Ks * traceRay(secondaryRay, depth+1));
}

//Computing Illuminations 
color3d Scene::computeLocalIllumination(hit_record rec, int depth, int samples_per_source){
    color3d Li = color3d(0.0f);
    auto lightCones = this->getLightCones(rec.pt, rec.mesh_index);
    for(unsigned int i = 0; i < lights.size(); i++){
        //Sampling in a cone made by the light object at the point of incidence
        double coneAngleCosine = lightCones[lights[i]];
        vec3d lightDir = meshes[lights[i]].centralVertex.pos - rec.pt;
        for(int j = 0; j < samples_per_source; j++){
            vec3d wi = sampleCone(unit_vec(lightDir),unit_vec(cross(lightDir,rec.normal)),coneAngleCosine);
            double cosTheta = dot(unit_vec(wi),rec.normal);
            if(cosTheta > 0){
                color3d kd = rec.mat.Kd;
                Ray shadowRay(rec.pt, wi);
                hit_record shadowRec = intersect(shadowRay);
                if(shadowRec.isHit){
                    if(shadowRec.mesh_index == lights[i]){
                        double lightCosTheta = dot(unit_vec(-wi), shadowRec.normal);
                        color3d Li_local = shadowRec.mat.Ke * color3d(1.0f) * lightCosTheta + shadowRec.mat.Ka * color3d(1.0f);
                        Li = (Li + ((kd * cosTheta * Li_local) / (samples_per_source)));
                    }
                }
            }
        }
    }
    return Li;
}

color3d Scene::computeGlobalIllumination(hit_record rec, int depth, int samples){
    color3d Li(0.0f);
    for(int i = 0; i < samples; i++){
        //Sampling in a hemisphere for indirect lighting 
        vec3d wi = sampleHemisphere(rec.normal, rec.tangent);
        double cosTheta = dot(wi,rec.normal);
        color3d kd = rec.mat.Kd;
        Ray secondaryRay(rec.pt, wi);
        color3d Li_local = traceRay(secondaryRay, depth+1);
        Li = Li + ((kd * cosTheta * Li_local) / (samples * (1 / (2 * M_PI))));
    }
    return Li;
}

//Processing intersections(without acceleraation)
hit_record Scene::intersect(Ray r){
    //First check for intersection with the bounding box
    bool isHit = intersectBox(r, 0.001, DBL_MAX, bvhRoot.bounds.nodeMin, bvhRoot.bounds.nodeMax);

    hit_record rec; 
    if(isHit){
        //Non zero minimum for handling numerical errors
        double t_min = 0.01;
        for(unsigned int i = 0; i < this->meshes.size(); i++){
            double t_max = rec.t;
            this->meshes[i].intersectBVH(r, t_min, t_max, i, rec, 0);
        } 
    }
    return rec; 
}

//Getting light cones from the given point
unordered_map<unsigned int, double> Scene::getLightCones(point3d p, int curr_index){
    unordered_map<unsigned int, double> light_cones; 
    for(unsigned int i = 0; i < this->lights.size(); i++){
        int lightIndex = lights[i];
        if(lightIndex != curr_index){
            double cone_angle_cosine = this->getLightCone(p, lightIndex); 
            light_cones.insert({lightIndex, cone_angle_cosine});
        }
    }
    return light_cones; 
}

double Scene::getLightCone(point3d p, int index){
    point3d centralVertex = this->meshes[index].centralVertex.pos;
    vec3d centralLightDir = unit_vec(centralVertex - p);
    double cosine = 1.0f;
    for(unsigned int i=0;i<this->meshes[index].vertices.size();i++){
        vec3d lightDir = unit_vec(this->meshes[index].vertices[i].pos - p);
        double newCosine = dot(centralLightDir, lightDir);
        cosine = min(cosine, newCosine);
    }
    return cosine;
}
