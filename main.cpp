#include<bits/stdc++.h>
#include "include/Scene.hpp"

using namespace std;

vector<vector<color3d>> pixelMap;

Scene generate_scene(string obj_path, string config_path){
    int width = 0;
    int height = 0;
    Scene newScene = Scene(obj_path, config_path, width, height);
    pixelMap = vector<vector<color3d>>(height,vector<color3d>(width,color3d()));
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

void display(){
    int h = pixelMap.size();
    int w = pixelMap[0].size();
    cout<<"P3\n"<<w<<" "<<h<<"\n255\n";
    for(int j=h-1;j>=0;j--){
        for(int i=0;i<w;i++){
            int r = clamp(pixelMap[j][i][0],0,255);
            int g = clamp(pixelMap[j][i][1],0,255);
            int b = clamp(pixelMap[j][i][2],0,255);
            cout << r << " " << g << " " << b << "\n";
        }
    }
}

int main(int argc, char** argv){

}