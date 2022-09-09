#include<bits/stdc++.h>
#include "include/Scene.hpp"

using namespace std;

vector<vector<color3d>> pixelMap;

Scene generate_scene(string obj_path, string config_path){
    Scene newScene(obj_path, config_path);
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

void display(int samples){
    int h = pixelMap.size();
    int w = pixelMap[0].size();
    double scale = 1.0/samples;
    cout<<"P3\n"<<w<<" "<<h<<"\n255\n";
    for(int j=h-1;j>=0;j--){
        for(int i=0;i<w;i++){
            color3d c = pixelMap[j][i];
            c = c*scale;
            c = color3d(clamp(sqrt(c[0]),0.0,0.999),clamp(sqrt(c[1]),0.0,0.999),clamp(sqrt(c[2]),0.0,0.999));
            int ir = int(256*c[0]);
            int ig = int(256*c[1]);
            int ib = int(256*c[2]);
            cout<<ir<<" "<<ig<<" "<<ib<<"\n";
        }
    }
}

int main(int argc, char** argv){
    string obj_path = argv[1];
    string config_path = argv[2];
    Scene newScene = generate_scene(obj_path, config_path);

    newScene.castRays(pixelMap,2,16);
    display(2);

    return 0;
}