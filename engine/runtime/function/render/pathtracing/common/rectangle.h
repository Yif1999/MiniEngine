#pragma once

#include "runtime/function/render/pathtracing/common/util.h"

#include "runtime/function/render/pathtracing/common/hittable.h"

namespace MiniEngine
{
    class RectangleXY : public Hittable
    {
    public:
        shared_ptr<Material> m;
        float x0, x1, y0, y1, k;

        RectangleXY(){};
        RectangleXY(float _x0, float _x1, float _y0, float _y1, float _k, shared_ptr<Material> mat) : x0(_x0), x1(_x1), y0(_y0), y1(_y1), k(_k), m(mat){};

        virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const override;
    };

    class RectangleXZ : public Hittable
    {
    public:
        shared_ptr<Material> m;
        float x0, x1, z0, z1, k;

        RectangleXZ() {}

        RectangleXZ(float _x0, float _x1, float _z0, float _z1, float _k, shared_ptr<Material> mat) : x0(_x0), x1(_x1), z0(_z0), z1(_z1), k(_k), m(mat){};

        virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const override;

        virtual float getPDF(const vec3& origin, const vec3& v) const override {
            HitRecord rec;
            if (!this->hit(Ray(origin, v), 0.001, INF, rec))
                return 0;

            auto area = (x1-x0)*(z1-z0);
            auto distance_squared = rec.t * rec.t * pow(length(v),2);
            auto cosine = fabs(dot(v, rec.normal) / length(v));

            return distance_squared / (cosine * area);
        }

        virtual vec3 random(const vec3& origin) const override {
            auto random_point = vec3(linearRand(x0,x1), k, linearRand(z0,z1));
            return random_point - origin;
        }
    };

    class RectangleYZ : public Hittable
    {
    public:
        shared_ptr<Material> m;
        float y0, y1, z0, z1, k;

        RectangleYZ() {}

        RectangleYZ(float _y0, float _y1, float _z0, float _z1, float _k, shared_ptr<Material> mat) : y0(_y0), y1(_y1), z0(_z0), z1(_z1), k(_k), m(mat){};

        virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const override;
    };

    bool RectangleXY::hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const
    {
        auto t = (k - r.origin.z) / r.direction.z;
        if (t < t_min || t > t_max)
            return false;
        auto x = r.origin.x + t * r.direction.x;
        auto y = r.origin.y + t * r.direction.y;
        if (x < x0 || x > x1 || y < y0 || y > y1)
            return false;
        rec.t = t;
        auto outward_normal = vec3(0, 0, 1);
        rec.setFaceNormal(r, outward_normal);
        rec.mat_ptr = m;
        rec.p = r.cast(t);
        return true;
    }

    bool RectangleXZ::hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const
    {
        auto t = (k - r.origin.y) / r.direction.y;
        if (t < t_min || t > t_max)
            return false;
        auto x = r.origin.x + t * r.direction.x;
        auto z = r.origin.z + t * r.direction.z;
        if (x < x0 || x > x1 || z < z0 || z > z1)
            return false;
        rec.t = t;
        auto outward_normal = vec3(0, 1, 0);
        rec.setFaceNormal(r, outward_normal);
        rec.mat_ptr = m;
        rec.p = r.cast(t);
        return true;
    }

    bool RectangleYZ::hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const
    {
        auto t = (k - r.origin.x) / r.direction.x;
        if (t < t_min || t > t_max)
            return false;
        auto y = r.origin.y + t * r.direction.y;
        auto z = r.origin.z + t * r.direction.z;
        if (y < y0 || y > y1 || z < z0 || z > z1)
            return false;
        rec.t = t;
        auto outward_normal = vec3(1, 0, 0);
        rec.setFaceNormal(r, outward_normal);
        rec.mat_ptr = m;
        rec.p = r.cast(t);
        return true;
    }
}
