#ifndef VEC_HPP
#define VEC_HPP

#include <bits/stdc++.h>
using namespace std;

class vec3d{

    public:
        //Constructors
        vec3d(){
            this->vec[0] = 0;
            this->vec[1] = 0;
            this->vec[2] = 0;
            this->magnitude = 0;
        }
        vec3d(double x){
            this->vec[0] = x;
            this->vec[1] = x;
            this->vec[2] = x;
            this->magnitude = sqrt(3) * x;
        }
        vec3d(double x,double y,double z){
            this->vec[0] = x;
            this->vec[1] = y;
            this->vec[2] = z;
            this->magnitude = sqrt(x*x+y*y+z*z);
        }

        //Access Methods
        double x(){return vec[0];}
        double y(){return vec[1];}
        double z(){return vec[2];}
        double length(){return magnitude;}

        //Set Methods
        void set(double x,double y,double z){
            this->vec[0] = x;
            this->vec[1] = y;
            this->vec[2] = z;
            this->magnitude = sqrt(x*x+y*y+z*z);
        }

        //Operator Overloading on 3D vectors
        double operator[](int t){
            return vec[t];
        }
        vec3d operator-(){
            return vec3d(-vec[0],-vec[1],-vec[2]);
        }
        vec3d& operator+=(vec3d v){
            vec[0] += v[0];
            vec[1] += v[1];
            vec[2] += v[2];
            return *this;
        }
        vec3d& operator-=(vec3d v){
            vec[0] -= v[0];
            vec[1] -= v[1];
            vec[2] -= v[2];
            return *this;
        }
        vec3d& operator*=(double t){
            vec[0] *= t;
            vec[1] *= t;
            vec[2] *= t;
            return *this;
        }
        vec3d& operator/=(double t){
            return *this *= (1/t);
        }
        bool operator==(vec3d v){
            return (this->vec[0]==v[0] && this->vec[1]==v[1] && this->vec[2]==v[2]);
        }

    private:
        double vec[3];
        double magnitude;

};

//Alternate aliases
using point3d = vec3d;
using color3d = vec3d;

//Static vector methods
ostream& operator<<(ostream& out,vec3d v);
vec3d operator+(vec3d u,vec3d v);
vec3d operator-(vec3d u,vec3d v);
vec3d operator*(vec3d v,double t);
vec3d operator*(double t,vec3d v);
vec3d operator*(vec3d u,vec3d v);
vec3d operator/(vec3d v,double t);
vec3d operator/(vec3d u,vec3d v);
vec3d unit_vec(vec3d v);
double dot(vec3d u,vec3d v);
vec3d cross(vec3d u,vec3d v);
double area(point3d p1,point3d p2,point3d p3);

//Reflection/Refraction vectors
vec3d reflect(vec3d incident_ray, vec3d normal);
vec3d refract(vec3d incident_ray, vec3d normal, double myu);

//Helper function for parsing
point3d readPoint(string s);

vec3d readVec(string s);

color3d readColor(string s);

//Utilities
double random_double();

#endif