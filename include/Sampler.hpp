#ifndef SAMPLER_HPP
#define SAMPLER_HPP

#include "vec3d.hpp"

vec3d sampleHemisphere(vec3d normal, vec3d tangent){
    //Obtaining the orthogonal axes in the point space
    vec3d y = normal;
    vec3d x = tangent;
    vec3d z = cross(x,y);

    //Sampling the hemishpere
    double phi = 2 * M_PI * random_double();
    double theta = acos(random_double());

    //Calculating the weights along each axis
    double x1 = sin(theta) * cos(phi); 
    double y1 = sin(theta) * sin(phi);
    double z1 = cos(theta);

    //Calculating the sampled direction
    return x1 * x + y1 * y + z1 * z;
}

#endif