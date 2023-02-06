#pragma once

#include "runtime/function/render/pathtracing/common/util.h"

namespace MiniEngine::PathTracing
{
    class AABB
    {
    public:
        glm::vec3 min;
        glm::vec3 max;

        AABB() {}
        AABB(const glm::vec3 &a, const glm::vec3 &b)
        {
            min = a;
            max = b;
        }

        bool hit(const Ray &r, float t_min, float t_max) const
        {
            for (int a = 0; a < 3; a++)
            {
                auto t0 = fmin((min[a] - r.origin[a]) / r.direction[a],
                               (max[a] - r.origin[a]) / r.direction[a]);
                auto t1 = fmax((min[a] - r.origin[a]) / r.direction[a],
                               (max[a] - r.origin[a]) / r.direction[a]);
                t_min = fmax(t0, t_min);
                t_max = fmin(t1, t_max);
                if (t_max <= t_min)
                    return false;
            }
            return true;
        }

        static AABB getSurroundingBox(AABB box0, AABB box1)
        {
            vec3 small(fmin(box0.min.x, box1.min.x),
                    fmin(box0.min.y, box1.min.y),
                    fmin(box0.min.z, box1.min.z));

            vec3 big(fmax(box0.max.x, box1.max.x),
                    fmax(box0.max.y, box1.max.y),
                    fmax(box0.max.z, box1.max.z));

            return AABB(small, big);
        }
    };

}