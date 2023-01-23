#pragma once

#include "runtime/function/render/pathtracing/ray.h"

struct HitRecord
{
    glm::vec3 p;
    glm::vec3 normal;
    float t;

    bool front_face;

    inline void setFaceNormal(const Ray &r, const vec3 &outward_normal)
    {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class Hittable
{
public:
    virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const = 0;
};