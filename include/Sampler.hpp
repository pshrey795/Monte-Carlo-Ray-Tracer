#ifndef SAMPLER_HPP
#define SAMPLER_HPP

#include "vec3d.hpp"

//Functions for sampling the omega vector given different constraints
vec3d sampleHemisphere(vec3d normal, vec3d tangent);
vec3d sampleCone(vec3d normal, vec3d tangent, double angle);



#endif