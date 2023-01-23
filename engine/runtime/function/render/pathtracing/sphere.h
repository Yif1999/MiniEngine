#pragma once

#include "runtime/function/render/pathtracing/hittable.h"

class Sphere : public Hittable
{
public:
    vec3 center;
    float radius;

public:
    Sphere() {}
    Sphere(glm::vec3 cen, float r) : center(cen), radius(r){};

    virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const override;
};

bool Sphere::hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const
{
    vec3 oc = r.origin - center;
    auto a = r.direction.length_squared();
    auto half_b = dot(oc, r.direction());
    auto c = oc.length_squared() - radius * radius;

    auto discriminant = half_b * half_b - a * c;
    if (discriminant < 0)
        return false;
    auto sqrtd = sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    auto root = (-half_b - sqrtd) / a;
    if (root < t_min || t_max < root)
    {
        root = (-half_b + sqrtd) / a;
        if (root < t_min || t_max < root)
            return false;
    }

    rec.t = root;
    rec.p = r.at(rec.t);
    vec3 outward_normal = (rec.p - center) / radius;
    rec.setFaceNormal(r, outward_normal);


    return true;
}