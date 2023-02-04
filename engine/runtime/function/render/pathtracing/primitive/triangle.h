#pragma once

#include "runtime/function/render/pathtracing/common/hittable.h"

namespace MiniEngine::PathTracing
{
    class Triangle : public Hittable
    {
    public:
        vector<vec3> vertices;
        shared_ptr<Material> mat_ptr;

        Triangle() {}
        Triangle(vector<vec3> vt, shared_ptr<Material> m) : vertices(vt), mat_ptr(m){};

        virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const override;
    };

    bool Triangle::hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const
    {
        // ray intersection
        vec3 edge1 = vertices[1] - vertices[0];
        vec3 edge2 = vertices[2] - vertices[0];

        auto q = cross(r.direction, edge2);
        auto a = dot(edge1, q);

        if (fabs(a) < EPS)
            return false;

        auto f = 1.0 / a;
        auto s = r.origin - vertices[0];
        auto u = f * dot(s, q);

        if (u < 0)
            return false;

        auto k = cross(s, edge1);
        auto v = f * dot(r.direction, k);

        if (v < 0 || u + v > 1)
            return false;

        auto t = f * dot(edge2, k);
        if (t < t_min || t_max < t)
            return false;

        rec.t = t;
        rec.p = r.cast(rec.t);
        vec3 outward_normal = normalize(cross(edge1, edge2));
        rec.setFaceNormal(r, outward_normal);
        rec.mat_ptr = mat_ptr;

        return true;
    }

}
