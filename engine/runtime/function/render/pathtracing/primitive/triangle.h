#pragma once

#include "runtime/function/render/pathtracing/common/hittable.h"

#include <glm/gtx/string_cast.hpp>

namespace MiniEngine::PathTracing
{
    class Triangle : public Hittable
    {
    public:
        vector<Vertex> vertices;
        shared_ptr<Material> mat_ptr;

        Triangle() {}
        Triangle(vector<Vertex> vt, shared_ptr<Material> m) : vertices(vt), mat_ptr(m){};

        virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const override;
        virtual bool aabb(AABB &output_box) const override;

        virtual float getPDF(const vec3 &origin, const vec3 &v) const override
        {
            HitRecord rec;
            if (!this->hit(Ray(origin, v), EPS, INF, rec))
                return 0;

            float area = getArea();

            float distance_squared = rec.t * rec.t * pow(length(v), 2);
            float cosine = fabs(dot(v, rec.hit_point.Normal) / length(v));

            return distance_squared / (cosine * area);
        }

        virtual vec3 random(const vec3 &origin) const override
        {

            vec3 edge1 = vertices[1].Position - vertices[0].Position;
            vec3 edge2 = vertices[2].Position - vertices[0].Position;

            auto u = linearRand(0.f, 1.f);
            auto v = linearRand(0.f, 1.f);

            vec3 random_point;

            if (u + v > 1)
                random_point = vertices[0].Position + (1 - u) * edge1 + (1 - v) * edge2;
            else
                random_point = vertices[0].Position + u * edge1 + v * edge2;

            return random_point - origin;
        }

        virtual float getArea() const override
        {
            vec3 edge1 = vertices[1].Position - vertices[0].Position;
            vec3 edge2 = vertices[2].Position - vertices[0].Position;

            return length(cross(edge1, edge2)) / 2.f;
        }

    private:
        vec2 interpTexcoord(float u, float v) const
        {
            vec2 st = u * vertices[1].Texcoord + v * vertices[2].Texcoord + (1 - u - v) * vertices[0].Texcoord;
            return st;
        }
    };

    bool Triangle::hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const
    {
        // ray intersection
        vec3 edge1 = vertices[1].Position - vertices[0].Position;
        vec3 edge2 = vertices[2].Position - vertices[0].Position;

        auto q = cross(r.direction, edge2);
        auto a = dot(edge1, q);

        if (fabs(a) < EPS)
            return false;

        auto f = 1.0 / a;
        auto s = r.origin - vertices[0].Position;
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
        rec.hit_point.Position = r.cast(rec.t);
        rec.hit_point.Texcoord = interpTexcoord(u, v);
        vec3 outward_normal = normalize(cross(edge1, edge2));
        rec.setFaceNormal(r, outward_normal);
        rec.mat_ptr = mat_ptr;

        return true;
    }

    bool Triangle::aabb(AABB &bounding_box) const
    {
        auto x_min = min({vertices[0].Position.x, vertices[1].Position.x, vertices[2].Position.x}) - EPS;
        auto y_min = min({vertices[0].Position.y, vertices[1].Position.y, vertices[2].Position.y}) - EPS;
        auto z_min = min({vertices[0].Position.z, vertices[1].Position.z, vertices[2].Position.z}) - EPS;

        auto x_max = max({vertices[0].Position.x, vertices[1].Position.x, vertices[2].Position.x}) + EPS;
        auto y_max = max({vertices[0].Position.y, vertices[1].Position.y, vertices[2].Position.y}) + EPS;
        auto z_max = max({vertices[0].Position.z, vertices[1].Position.z, vertices[2].Position.z}) + EPS;

        bounding_box = AABB(vec3(x_min, y_min, z_min), vec3(x_max, y_max, z_max));

        return true;
    }

}
