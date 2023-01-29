#pragma once

#include "runtime/function/render/pathtracing/common/rectangle.h"
#include "runtime/function/render/pathtracing/common/util.h"
#include "runtime/function/render/pathtracing/common/hittable.h"

namespace MiniEngine
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
}