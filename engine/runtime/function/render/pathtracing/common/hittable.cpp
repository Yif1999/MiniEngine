#include "runtime/function/render/pathtracing/common/hittable.h"

namespace MiniEngine::PathTracing
{
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

    float HittableList::getPDF(const vec3 &o, const vec3 &v) const
    {
        auto weight = 1.0 / objects.size();
        auto sum = 0.0;

        for (const auto &object : objects)
            sum += weight * object->getPDF(o, v);

        return sum;
    }

    vec3 HittableList::random(const vec3 &o) const
    {
        auto int_size = static_cast<int>(objects.size());
        return objects[int(linearRand(0, int_size - 1) + 0.5)]->random(o);
    }

    bool HittableList::aabb(AABB &bounding_box) const
    {
        if (objects.empty())
            return false;

        AABB temp_box;
        bool first_box = true;

        for (const auto &object : objects)
        {
            if (!object->aabb(temp_box))
                return false;
            bounding_box = first_box ? temp_box : AABB::getSurroundingBox(bounding_box, temp_box);
            first_box = false;
        }

        return true;
    }
}