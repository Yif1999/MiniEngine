#pragma once

#include "runtime/function/render/pathtracing/common/util.h"

namespace MiniEngine::PathTracing
{
    class ONB
    {
    public:
        vec3 axis[3];

        ONB() {}

        vec3 local(float a, float b, float c) const
        {
            return a * axis[0] + b * axis[1] + c * axis[2];
        }

        vec3 local(const vec3 &a) const
        {
            return a.x * axis[0] + a.y * axis[1] + a.z * axis[2];
        }

        void buildONB(const vec3 &);

    public:
    };

    void ONB::buildONB(const vec3 &n)
    {
        axis[2] = normalize(n);
        vec3 a = (fabs(axis[2].x) > 0.9) ? vec3(0, 1, 0) : vec3(1, 0, 0);
        axis[1] = normalize(cross(axis[2], a));
        axis[0] = cross(axis[2], axis[1]);
    }
}
