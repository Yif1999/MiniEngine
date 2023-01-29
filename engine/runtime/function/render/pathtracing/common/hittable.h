#pragma once

#include "runtime/function/render/pathtracing/common/util.h"

namespace MiniEngine
{
    class Material;

    struct HitRecord
    {
        glm::vec3 p;
        glm::vec3 normal;
        shared_ptr<Material> mat_ptr;
        float t;

        bool front_face;

        inline void setFaceNormal(const Ray &r, const vec3 &outward_normal)
        {
            front_face = dot(r.direction, outward_normal) < 0;
            normal = front_face ? outward_normal : -outward_normal;
        }
    };

    class Hittable
    {
    public:
        virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const = 0;

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
        HittableList() {}
        HittableList(shared_ptr<Hittable> object) { add(object); }

        void clear() { objects.clear(); }
        void add(shared_ptr<Hittable> object) { objects.push_back(object); }

        virtual bool hit(
            const Ray &r, float t_min, float t_max, HitRecord &rec) const override;

    public:
        vector<shared_ptr<Hittable>> objects;
    };

    bool HittableList::hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const
    {
        HitRecord temp_rec;
        bool hit_anything = false;
        auto closest_so_far = t_max;

        for (const auto &object : objects)
        {
            if (object->hit(r, t_min, closest_so_far, temp_rec))
            {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }

    class Translate : public Hittable
    {
    public:
        shared_ptr<Hittable> ptr;
        vec3 offset;

        Translate(shared_ptr<Hittable> p, const vec3 &displacement) : ptr(p), offset(displacement) {}

        virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const override;
    };

    bool Translate::hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const
    {
        Ray moved_r(r.origin - offset, r.direction);
        if (!ptr->hit(moved_r, t_min, t_max, rec))
            return false;

        rec.p += offset;
        rec.setFaceNormal(moved_r, rec.normal);

        return true;
    }

    class RotateY : public Hittable
    {
    public:
        shared_ptr<Hittable> ptr;
        float sin_theta;
        float cos_theta;

        RotateY(shared_ptr<Hittable> p, float angle);

        virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const override;
    };

    RotateY::RotateY(shared_ptr<Hittable> p, float angle) : ptr(p)
    {
        auto radians = glm::radians(angle);
        sin_theta = sin(radians);
        cos_theta = cos(radians);
    }

    bool RotateY::hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const
    {
        auto origin = r.origin;
        auto direction = r.direction;

        origin[0] = cos_theta * r.origin[0] - sin_theta * r.origin[2];
        origin[2] = sin_theta * r.origin[0] + cos_theta * r.origin[2];

        direction[0] = cos_theta * r.direction[0] - sin_theta * r.direction[2];
        direction[2] = sin_theta * r.direction[0] + cos_theta * r.direction[2];

        Ray rotated_r(origin, direction);

        if (!ptr->hit(rotated_r, t_min, t_max, rec))
            return false;

        auto p = rec.p;
        auto normal = rec.normal;

        p[0] = cos_theta * rec.p[0] + sin_theta * rec.p[2];
        p[2] = -sin_theta * rec.p[0] + cos_theta * rec.p[2];

        normal[0] = cos_theta * rec.normal[0] + sin_theta * rec.normal[2];
        normal[2] = -sin_theta * rec.normal[0] + cos_theta * rec.normal[2];

        rec.p = p;
        rec.setFaceNormal(rotated_r, normal);

        return true;
    }

}
