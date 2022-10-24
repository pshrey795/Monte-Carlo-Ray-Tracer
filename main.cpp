#include<bits/stdc++.h>
#include "include/Scene.hpp"

using namespace std;

vector<vector<color3d>> pixelMap;

//Default parameters
int nthreads = 4;
int samples_per_pixel = 2;
int direct_samples_per_ray = 5;
int indirect_samples_per_ray = 5;
int tracing_depth = 3;
int min_primitives_per_box = 2;

void parseParameters(string config_path){
    ifstream myFile(config_path);
    string line;

    getline(myFile, line);
    while(line.substr(0,line.length()-1) != "parameters"){
        getline(myFile, line);
    }

    //Obtaining the ray tracing parameters
    getline(myFile, line);
    int currLen = line.length();
    nthreads = stoi(line.substr(14, currLen - 14));
    getline(myFile, line);
    currLen = line.length();
    samples_per_pixel = stoi(line.substr(23, currLen - 23));
    getline(myFile, line);
    currLen = line.length();
    direct_samples_per_ray = stoi(line.substr(28, currLen - 28));
    getline(myFile, line);
    currLen = line.length();
    indirect_samples_per_ray = stoi(line.substr(30, currLen - 30));
    getline(myFile, line);
    currLen = line.length();
    tracing_depth = stoi(line.substr(19, currLen - 19));
    getline(myFile, line);
    currLen = line.length();
    min_primitives_per_box = stoi(line.substr(28, currLen - 28));

    myFile.close();
}

Scene generate_scene(string obj_path, string config_path){
    parseParameters(config_path);
    Scene newScene(obj_path, config_path, direct_samples_per_ray, indirect_samples_per_ray, tracing_depth, min_primitives_per_box);
    return newScene;
}

double clamp(double a,double low, double high){
    if(a<low){
        return low;
    }else if(a>high){
        return high;
    }else{
        return a;
    }
}

void display(string outputFile){
    int h = pixelMap.size();
    int w = pixelMap[0].size();
    double scale = 1.0/(samples_per_pixel);
    ofstream myFile(outputFile);
    myFile<<"P3\n"<<w<<" "<<h<<"\n255\n";
    for(int j=h-1;j>=0;j--){
        for(int i=0;i<w;i++){
            color3d c = pixelMap[j][i];
            c = c * scale;
            //Clamping and Gamma correction
            double r = sqrt(clamp(c[0], 0.0, 0.999));
            double g = sqrt(clamp(c[1], 0.0, 0.999));
            double b = sqrt(clamp(c[2], 0.0, 0.999));

            //Scaling for ppm format
            int ir = int(255.99*r);
            int ig = int(255.99*g);
            int ib = int(255.99*b);
            myFile<<ir<<" "<<ig<<" "<<ib<<"\n";
        }
    }
    myFile.close();
}

int main(int argc, char** argv){
    string obj_path = argv[1];
    string config_path = argv[2];
    Scene newScene = generate_scene(obj_path, config_path);

    auto start = chrono::high_resolution_clock::now();
    newScene.castRays(pixelMap,samples_per_pixel,nthreads);
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
    ofstream execFile(argv[4]);
    execFile << duration.count() << endl;
    execFile.close();

    display(argv[3]);

    return 0;
}