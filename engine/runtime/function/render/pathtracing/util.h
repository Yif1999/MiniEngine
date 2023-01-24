#pragma once

// Headers

#include <cmath>
#include <limits>
#include <memory>
#include <vector>
#include <cstdlib>

#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>

#include "runtime/core/math/math.h"
#include "runtime/function/render/pathtracing/ray.h"

// Usings

using namespace std;
using namespace glm;

// Constants

const float INF = numeric_limits<float>::infinity();
const float PI = 3.1415926535897932385;
const float Eps = 1e-8;

// Utility Functions

bool isNearZero(vec3 v)
{
    return (fabs(v.x) < Eps) && (fabs(v.y) < Eps) && (fabs(v.z) < Eps);
}