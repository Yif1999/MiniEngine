#pragma once

#include "runtime/function/render/pathtracing/util.h"

namespace MiniEngine
{
    struct HitRecord;

    class Material
    {
    public:
        virtual bool scatter(
            const Ray &r_in, const HitRecord &rec, vec3 &attenuation, Ray &scattered) const = 0;
    };

    class Diffuse : public Material
    {
    public:
        vec3 albedo;

        Diffuse(const vec3 &a) : albedo(a) {}

        virtual bool scatter(const Ray &r_in, const HitRecord &rec, vec3 &attenuation, Ray &scattered) const override
        {
            auto scatter_direction = rec.normal + sphericalRand(1.f);

            if (isNearZero(scatter_direction))
                scatter_direction = rec.normal;

            scattered = Ray(rec.p, scatter_direction);
            attenuation = albedo;
            return true;
        }
    };

    class Metal : public Material
    {
    public:
        vec3 albedo;
        float fuzz;

        Metal(const vec3 &a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

        virtual bool scatter(
            const Ray &r_in, const HitRecord &rec, vec3 &attenuation, Ray &scattered) const override
        {
            vec3 reflected = reflect(r_in.direction, rec.normal);
            scattered = Ray(rec.p, reflected + fuzz * sphericalRand(1.f));
            attenuation = albedo;
            return (dot(scattered.direction, rec.normal) > 0);
        }
    };

}