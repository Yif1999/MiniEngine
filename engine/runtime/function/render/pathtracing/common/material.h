#pragma once

#include "runtime/function/render/pathtracing/common/util.h"
#include "runtime/function/render/pathtracing/common/onb.h"

namespace MiniEngine
{
    struct HitRecord;

    class Material
    {
    public:
        virtual bool scatter(const Ray &r_in, const HitRecord &rec, vec3 &attenuation, Ray &scattered) const
        {
            return false;
        }

        virtual bool scatter(const Ray &r_in, const HitRecord &rec, vec3 &attenuation, Ray &scattered, float &pdf) const
        {
            return false;
        }

        virtual float scatterPDF(const Ray &r_in, const HitRecord &rec, const Ray &scattered) const
        {
            return 0;
        }

        virtual vec3 emitted(const HitRecord &rec) const
        {
            return vec3(0, 0, 0);
        }
    };

    class Lambertian : public Material
    {
    public:
        vec3 albedo;

        Lambertian(const vec3 &a) : albedo(a) {}

        virtual bool scatter(const Ray &r_in, const HitRecord &rec, vec3 &attenuation, Ray &scattered, float &pdf) const override
        {
            ONB onb;
            onb.buildONB(rec.normal);
            auto direction = onb.local(cosineRand());
            scattered = Ray(rec.p, normalize(direction));
            attenuation = albedo;
            pdf = dot(onb.axis[2], scattered.direction) / PI;
            return true;
        }

        float scatterPDF(const Ray &r_in, const HitRecord &rec, const Ray &scattered) const override
        {
            auto cosine = dot(rec.normal, normalize(scattered.direction));
            return cosine < 0 ? 0 : cosine / PI;
        }
    };

    class Metal : public Material
    {
    public:
        vec3 albedo;
        float fuzz;

        Metal(const vec3 &a, float f) : albedo(a), fuzz(f < 1 ? f : 1) {}

        virtual bool scatter(
            const Ray &r_in, const HitRecord &rec, vec3 &attenuation, Ray &scattered) const override
        {
            vec3 unit_direction = normalize(r_in.direction);
            vec3 reflected = reflect(unit_direction, rec.normal);
            scattered = Ray(rec.p, reflected + fuzz * sphericalRand(1.f));
            attenuation = albedo;
            return (dot(scattered.direction, rec.normal) > 0);
        }
    };

    class Dielectric : public Material
    {
    public:
        float ir;

        Dielectric(float index_of_refraction) : ir(index_of_refraction) {}

        virtual bool scatter(
            const Ray &r_in, const HitRecord &rec, vec3 &attenuation, Ray &scattered) const override
        {
            attenuation = vec3(1.0, 1.0, 1.0);
            float refraction_ratio = rec.front_face ? (1.0 / ir) : ir;

            vec3 unit_direction = normalize(r_in.direction);
            float cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
            float sin_theta = sqrt(1.0 - cos_theta * cos_theta);

            bool cannot_refract = refraction_ratio * sin_theta > 1.0;
            vec3 direction;

            if (cannot_refract || reflectance(cos_theta, refraction_ratio) > linearRand(0.f, 1.f))
                direction = reflect(unit_direction, rec.normal);
            else
                direction = refract(unit_direction, rec.normal, refraction_ratio);

            scattered = Ray(rec.p, direction);

            return true;
        }

    private:
        static float reflectance(float cosine, float ref_idx)
        {
            // Use Schlick's approximation for reflectance.
            auto r0 = (1 - ref_idx) / (1 + ref_idx);
            r0 = r0 * r0;
            return r0 + (1 - r0) * pow((1 - cosine), 5);
        }
    };

    class Emission : public Material
    {
    public:
        vec3 emit;

        Emission(vec3 c) : emit(c) {}

        virtual bool scatter(const Ray &r_in, const HitRecord &rec, vec3 &attenuation, Ray &scattered) const override
        {
            return false;
        }

        virtual vec3 emitted(const HitRecord &rec) const override
        {
            if (!rec.front_face)
                return emit;
            else
                return vec3(0,0,0);
        }
    };

}