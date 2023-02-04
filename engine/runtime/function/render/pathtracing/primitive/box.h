#pragma once

#include "runtime/function/render/pathtracing/primitive/rectangle.h"
#include "runtime/function/render/pathtracing/common/util.h"
#include "runtime/function/render/pathtracing/common/hittable.h"

namespace MiniEngine::PathTracing
{
    class Box : public Hittable
    {
    public:
        vec3 box_min;
        vec3 box_max;
        HittableList sides;

    
        Box() {}
        Box(const vec3 &p0, const vec3 &p1, shared_ptr<Material> ptr);

        virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const override;
    };

    Box::Box(const vec3 &p0, const vec3 &p1, shared_ptr<Material> ptr)
    {
        box_min = p0;
        box_max = p1;

        sides.add(make_shared<RectangleXY>(p0.x, p1.x, p0.y, p1.y, p1.z, ptr));
        sides.add(make_shared<RectangleXY>(p0.x, p1.x, p0.y, p1.y, p0.z, ptr));

        sides.add(make_shared<RectangleXZ>(p0.x, p1.x, p0.z, p1.z, p1.y, ptr));
        sides.add(make_shared<RectangleXZ>(p0.x, p1.x, p0.z, p1.z, p0.y, ptr));

        sides.add(make_shared<RectangleYZ>(p0.y, p1.y, p0.z, p1.z, p1.x, ptr));
        sides.add(make_shared<RectangleYZ>(p0.y, p1.y, p0.z, p1.z, p0.x, ptr));
    }

    bool Box::hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const
    {
        return sides.hit(r, t_min, t_max, rec);
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