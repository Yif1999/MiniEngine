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

    class Lambertian : public Material
    {
    public:
        vec3 albedo;

        Lambertian(const vec3 &a) : albedo(a) {}

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

        Dielectric(double index_of_refraction) : ir(index_of_refraction) {}

        virtual bool scatter(
            const Ray &r_in, const HitRecord &rec, vec3 &attenuation, Ray &scattered) const override
        {
            attenuation = vec3(1.0, 1.0, 1.0);
            float refraction_ratio = rec.front_face ? (1.0 / ir) : ir;

            vec3 unit_direction = normalize(r_in.direction);
            double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
            double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

            bool cannot_refract = refraction_ratio * sin_theta > 1.0;
            vec3 direction;

            if (cannot_refract || reflectance(cos_theta, refraction_ratio) > linearRand(0.f,1.f))
                direction = reflect(unit_direction, rec.normal);
            else
                direction = refract(unit_direction, rec.normal, refraction_ratio);

            scattered = Ray(rec.p, direction);

            return true;
        }

    private:
        static double reflectance(double cosine, double ref_idx)
        {
            // Use Schlick's approximation for reflectance.
            auto r0 = (1 - ref_idx) / (1 + ref_idx);
            r0 = r0 * r0;
            return r0 + (1 - r0) * pow((1 - cosine), 5);
        }
    };

}