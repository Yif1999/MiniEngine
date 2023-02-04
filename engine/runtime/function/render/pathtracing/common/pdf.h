#pragma once

#include "runtime/function/render/pathtracing/common/util.h"
#include "runtime/function/render/pathtracing/common/hittable.h"

namespace MiniEngine::PathTracing
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

    class MixturePDF : public PDF
    {
    public:
        shared_ptr<PDF> p[2];

        MixturePDF(shared_ptr<PDF> p0, shared_ptr<PDF> p1)
        {
            p[0] = p0;
            p[1] = p1;
        }

        virtual float value(const vec3 &direction) const override
        {
            return 0.5 * p[0]->value(direction) + 0.5 * p[1]->value(direction);
        }

        virtual vec3 generate() const override
        {
            if (linearRand(0.f,1.f) < 0.5)
                return p[0]->generate();
            else
                return p[1]->generate();
        }

    };
}