#include "../include/Sampler.hpp"

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
    double y1 = cos(theta);
    double z1 = sin(theta) * sin(phi);

    //Calculating the sampled direction
    return unit_vec(x1 * x + y1 * y + z1 * z);
}

vec3d sampleCone(vec3d normal, vec3d tangent, double angleCosine){
    //Obtaining the orthogonal axes in the point space
    vec3d y = normal;
    vec3d x = tangent;
    vec3d z = cross(x,y);

    //Sampling the cone
    double phi = 2 * M_PI * random_double();
    double k = random_double();
    double theta = acos((1-k) + k * angleCosine);

    //Calculating the weights along each axis
    double x1 = sin(theta) * cos(phi); 
    double y1 = cos(theta);
    double z1 = sin(theta) * sin(phi);

    //Calculating the sampled direction
    return unit_vec(x1 * x + y1 * y + z1 * z);
}

vec3d sampleWeightedHemisphere(vec3d normal, vec3d tangent){
    //Obtaining the orthogonal axes in the point space
    vec3d y = normal;
    vec3d x = tangent;
    vec3d z = cross(x,y);

    //Sampling the cone
    double phi = 2 * M_PI * random_double();
    double theta = asin(sqrt(random_double()));

    //Calculating the weights along each axis
    double x1 = sin(theta) * cos(phi); 
    double y1 = sin(theta) * sin(phi);
    double z1 = cos(theta);

    //Calculating the sampled direction
    return unit_vec(x1 * x + y1 * y + z1 * z);
}

vec3d sampleTriangle(point3d p1, point3d p2, point3d p3){
    vec3d U = p2 - p1;
    vec3d V = p3 - p1;

    double r1 = random_double();
    double r2 = random_double();
    double u = 1 - sqrt(r1);
    double v = r2 * sqrt(r1);

    return p1 + u * U + v * V;
}