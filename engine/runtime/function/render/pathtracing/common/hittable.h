#pragma once

#include "runtime/function/render/pathtracing/common/util.h"
#include "runtime/function/render/pathtracing/acc_struct/aabb.h"
#include "runtime/function/render/render_mesh.h"

namespace MiniEngine::PathTracing
{
    class Material;

    struct HitRecord
    {
        MiniEngine::Vertex hit_point;
        shared_ptr<Material> mat_ptr;
        float t;
        bool front_face;

        inline void setFaceNormal(const Ray &r, const vec3 &outward_normal)
        {
            front_face = dot(r.direction, outward_normal) < 0;
            hit_point.Normal = front_face ? outward_normal : -outward_normal;
        }
    };

    class Hittable
    {
    public:
        virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const = 0;
        virtual bool aabb(AABB &bounding_box) const = 0;

        virtual float getArea() const
        {
            return 0.0;
        }

        virtual float getPDF(const vec3 &o, const vec3 &v) const
        {
            return 0.0;
        }

        virtual vec3 random(const vec3 &o) const
        {
            return vec3(1, 0, 0);
        }
    };

    class HittableList : public Hittable
    {
    public:
        vector<shared_ptr<Hittable>> objects;

        HittableList() {}
        HittableList(shared_ptr<Hittable> object) { add(object); }

        void clear() { objects.clear(); }
        void add(shared_ptr<Hittable> object) { objects.push_back(object); }

        virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const override;
        virtual bool aabb(AABB &bounding_box) const override;
        virtual float getPDF(const vec3 &o, const vec3 &v) const override;
        virtual vec3 random(const vec3 &o) const override;
    };

}
