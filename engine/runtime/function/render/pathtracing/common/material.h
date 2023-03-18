#pragma once

#include "runtime/function/render/pathtracing/common/util.h"
#include "runtime/function/render/pathtracing/common/onb.h"
#include "runtime/function/render/pathtracing/common/pdf.h"
#include "runtime/function/render/render_mesh.h"
#include "runtime/function/render/render_texture.h"

namespace MiniEngine::PathTracing
{
    struct HitRecord;

    struct ScatterRecord
    {
        Ray specular_ray;
        bool is_specular;
        vec3 attenuation;
        shared_ptr<PDF> pdf_ptr;
    };

    class Material
    {
    public:
        virtual bool scatter(const Ray &r_in, const HitRecord &rec, ScatterRecord &srec) const
        {
            return false;
        }

        virtual float scatterPDF(const Ray &r_in, const HitRecord &rec, const Ray &scattered) const
        {
            return 0;
        }

        virtual vec3 emitted(const Ray &r_in, const HitRecord &rec) const
        {
            return vec3(0, 0, 0);
        }
    };

    class Lambertian : public Material
    {
    public:
        vec3 albedo;

        Lambertian(const vec3 &a) : albedo(a) {}

        virtual bool scatter(const Ray &r_in, const HitRecord &rec, ScatterRecord &srec) const override
        {
            srec.is_specular = false;
            srec.attenuation = albedo;
            srec.pdf_ptr = make_shared<CosinePDF>(rec.hit_point.Normal);
            return true;
        }

        float scatterPDF(const Ray &r_in, const HitRecord &rec, const Ray &scattered) const override
        {
            auto cosine = dot(rec.hit_point.Normal, scattered.direction);
            return cosine < 0 ? 0 : cosine / PI;
        }
    };

    class Metal : public Material
    {
    public:
        vec3 albedo;
        float fuzz;

        Metal(const vec3 &a, float f) : albedo(a), fuzz(f < 1 ? f : 1) {}

        virtual bool scatter(const Ray &r_in, const HitRecord &rec, ScatterRecord &srec) const override
        {
            vec3 reflected = reflect(r_in.direction, rec.hit_point.Normal);
            srec.specular_ray = Ray(rec.hit_point.Position, reflected + fuzz * sphericalRand(1.f));
            srec.attenuation = albedo;
            srec.is_specular = true;
            srec.pdf_ptr = 0;
            return true;
        }
    };

    class Dielectric : public Material
    {
    public:
        float ir;

        Dielectric(float index_of_refraction) : ir(index_of_refraction) {}

        virtual bool scatter(const Ray &r_in, const HitRecord &rec, ScatterRecord &srec) const override
        {
            srec.is_specular = true;
            srec.pdf_ptr = nullptr;
            srec.attenuation = vec3(1.0, 1.0, 1.0);
            float refraction_ratio = rec.front_face ? (1.0 / ir) : ir;

            vec3 unit_direction = normalize(r_in.direction);
            float cos_theta = fmin(dot(-unit_direction, rec.hit_point.Normal), 1.0);
            float sin_theta = sqrt(1.0 - cos_theta * cos_theta);

            bool cannot_refract = refraction_ratio * sin_theta > 1.0;
            vec3 direction;

            if (cannot_refract || reflectance(cos_theta, refraction_ratio) > linearRand(0.f, 1.f))
                direction = reflect(unit_direction, rec.hit_point.Normal);
            else
                direction = refract(unit_direction, rec.hit_point.Normal, refraction_ratio);

            srec.specular_ray = Ray(rec.hit_point.Position, direction);

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

        virtual bool scatter(const Ray &r_in, const HitRecord &rec, ScatterRecord &srec) const override
        {
            return false;
        }

        virtual vec3 emitted(const Ray &r_in, const HitRecord &rec) const override
        {
            if (rec.front_face)
                return emit;
            else
                return vec3(0, 0, 0);
        }
    };

    class Phong : public Material
    {
    public:
        MiniEngine::Material mat;
        shared_ptr<Texture> diffuse_map;

        Phong(MiniEngine::Material m, std::string path)
        {
            this->mat = m;
            diffuse_map = make_shared<Image>((path + "/" + mat.map_Kd).c_str());
        }

        virtual bool scatter(const Ray &r_in, const HitRecord &rec, ScatterRecord &srec) const override
        {
            if (is_emitted(mat))
            {
                return false;
            }

            if (is_specular(mat) && linearRand(0.f, 1.f) < 0.5f)
            {
                vec3 reflected = reflect(r_in.direction, rec.hit_point.Normal);
                vec3 noise = 1.f / log(mat.Ns) * ballRand(1.f);

                vec3 normal = normalize(rec.hit_point.Normal);
                vec3 tangent = normalize(cross(normal, cross(reflected, normal)));
                vec3 bi_tangent = normalize(cross(normal, tangent));

                f32 noise_n = dot(noise, normal);
                f32 noise_t = dot(noise, tangent);
                f32 noise_bt = dot(noise, bi_tangent);

                noise_t = noise_t / dot(normalize(reflected), normal);
                noise_bt = noise_bt * dot(normalize(reflected), normal);

                noise = normal * noise_n + tangent * noise_t + bi_tangent * noise_bt;

                srec.specular_ray = Ray(rec.hit_point.Position, reflected + noise);
                srec.attenuation = mat.Ks;
                srec.is_specular = true;
                srec.pdf_ptr = nullptr;

                return true;
            }

            if (is_transparent(mat))
            {
                float refraction_ratio = rec.front_face ? (1.0 / mat.Ni) : mat.Ni;

                vec3 unit_direction = normalize(r_in.direction);
                float cos_theta = fmin(dot(-unit_direction, rec.hit_point.Normal), 1.0);
                float sin_theta = sqrt(1.0 - cos_theta * cos_theta);

                bool cannot_refract = refraction_ratio * sin_theta > 1.0;
                vec3 direction;

                if (cannot_refract || reflectance(cos_theta, refraction_ratio) > linearRand(0.f, 1.f))
                    direction = reflect(unit_direction, rec.hit_point.Normal);
                else
                    direction = refract(unit_direction, rec.hit_point.Normal, refraction_ratio);

                srec.is_specular = true;
                srec.pdf_ptr = nullptr;
                srec.attenuation = mat.Tr;
                srec.specular_ray = Ray(rec.hit_point.Position, direction);

                return true;
            }

            auto attenuation =  diffuse_map->value(rec.hit_point.Texcoord.s, rec.hit_point.Texcoord.t, rec.hit_point.Position);
            if (attenuation[0]<0)
            {
                srec.attenuation = mat.Kd;
            }
            else
            {
                srec.attenuation = attenuation;
            }

            srec.is_specular = false;
            srec.pdf_ptr = make_shared<CosinePDF>(rec.hit_point.Normal);
            return true;
        }

        virtual vec3 emitted(const Ray &r_in, const HitRecord &rec) const override
        {
            if (is_emitted(mat) && rec.front_face)
                return mat.Ke;
            else
                return vec3(0, 0, 0);
        }

        float scatterPDF(const Ray &r_in, const HitRecord &rec, const Ray &scattered) const override
        {
            auto cosine = dot(rec.hit_point.Normal, scattered.direction);
            return cosine < 0 ? 0 : cosine / PI;
        }

        inline bool is_transparent(MiniEngine::Material mat) const
        {
            if (mat.Ni > 1)
            {
                return true;
            }
            return false;
        }

        inline bool is_emitted(MiniEngine::Material mat) const
        {
            if (mat.Ke[0] > 0 || mat.Ke[1] > 0 || mat.Ke[2] > 0)
            {
                return true;
            }
            return false;
        }

        inline bool is_specular(MiniEngine::Material mat) const
        {
            if (mat.Ks[0] > 0 || mat.Ks[1] > 0 || mat.Ks[2] > 0)
            {
                return true;
            }
            return false;
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

}