#pragma once

// Headers

#include <cmath>
#include <limits>
#include <memory>
#include <vector>
#include <cstdlib>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>

#include "runtime/core/math/math.h"
#include "runtime/function/render/pathtracing/common/ray.h"

namespace MiniEngine::PathTracing
{
    // Usings

    using namespace std;
    using namespace glm;

    // Constants

    const float INF = numeric_limits<float>::infinity();
    const float PI = 3.1415926535897932385;
    const float EPS = 1e-5;

    // Utility Functions

    inline bool isNearZero(vec3 v)
    {
        return (fabs(v.x) < EPS) && (fabs(v.y) < EPS) && (fabs(v.z) < EPS);
    }

    inline bool isInfinity(vec3 v)
    {
        return (fabs(v.x) == INF) && (fabs(v.y) == INF) && (fabs(v.z) == INF);
    }

    inline bool isNan(vec3 v)
    {
        return (isnan(v.x)) && (isnan(v.y)) && (isnan(v.z));
    }

    inline vec3 cosineRand()
    {
        auto r1 = linearRand(0.f, 1.f);
        auto r2 = linearRand(0.f, 1.f);
        auto z = sqrt(1 - r2);

        auto phi = 2 * PI * r1;
        auto x = cos(phi) * sqrt(r2);
        auto y = sin(phi) * sqrt(r2);

        return vec3(x, y, z);
    }

}
