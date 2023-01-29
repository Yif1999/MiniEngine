#pragma once

#include "runtime/function/render/pathtracing/common/util.h"
#include "runtime/function/render/pathtracing/common/hittable.h"

namespace MiniEngine
{
    class PDF
    {
    public:
        virtual ~PDF() {}

        virtual float value(const vec3 &direction) const = 0;
        virtual vec3 generate() const = 0;
    };

    class CosinePDF : public PDF
    {
    public:
        ONB onb;

        CosinePDF(const vec3 &normal)
        {
            onb.buildONB(normal);
        }

        virtual float value(const vec3 &direction) const override
        {
            auto cosine = dot(normalize(direction), onb.axis[2]);
            return (cosine <= 0) ? 0 : cosine / PI;
        }

        virtual vec3 generate() const override
        {
            return onb.local(cosineRand());
        }
    };

    class HittablePDF : public PDF
    {
    public:
        vec3 o;
        shared_ptr<Hittable> ptr;

        HittablePDF(shared_ptr<Hittable> p, const vec3 &origin) : ptr(p), o(origin) {}

        virtual float value(const vec3 &direction) const override
        {
            return ptr->getPDF(o, direction);
        }

        virtual vec3 generate() const override
        {
            return ptr->random(o);
        }

    };
}