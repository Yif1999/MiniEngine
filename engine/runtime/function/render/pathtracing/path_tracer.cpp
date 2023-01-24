#include "runtime/function/render/pathtracing/path_tracer.h"
#include "runtime/function/render/pathtracing/util.h"
#include "runtime/function/render/pathtracing/sphere.h"
#include "runtime/function/render/pathtracing/camera.h"
#include "runtime/function/render/pathtracing/material.h"

namespace MiniEngine
{

    vec3 getColor(const Ray &r, const Hittable &model, int depth)
    {
        HitRecord rec;

        if (depth <= 0)
            return vec3(0, 0, 0);

        if (model.hit(r, 0.001, INF, rec))
        {
            Ray scattered;
            vec3 attenuation;
            if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
                return attenuation * getColor(scattered, model, depth - 1);
            return vec3(0, 0, 0);
        }
        vec3 unit_direction = r.direction;
        normalize(unit_direction);
        auto t = 0.5 * (unit_direction.y + 1.0);
        return f32(1.0 - t) * vec3(1.0, 1.0, 1.0) + f32(t) * vec3(0.5, 0.7, 1.0);
    }

    void PathTracer::startRender(unsigned char *pixels)
    {
        int window_size = 512;

        // Image
        const int image_width = window_size;
        const int image_height = window_size;
        const int samples = 100;
        const int max_depth = 50;

        // Camera
        Camera cam;

        // Model
        HittableList model;

        auto material_ground = make_shared<Diffuse>(vec3(0.8, 0.8, 0.0));
        auto material_center = make_shared<Diffuse>(vec3(0.7, 0.3, 0.3));
        auto material_left   = make_shared<Metal>(vec3(0.8, 0.8, 0.8), 0.3);
        auto material_right  = make_shared<Metal>(vec3(0.8, 0.6, 0.2), 1.0);

        model.add(make_shared<Sphere>(vec3(0.0, -100.5, -1.0), 100.0, material_ground));
        model.add(make_shared<Sphere>(vec3(0.0, 0.0, -1.0), 0.5, material_center));
        model.add(make_shared<Sphere>(vec3(-1.0, 0.0, -1.0), 0.5, material_left));
        model.add(make_shared<Sphere>(vec3(1.0, 0.0, -1.0), 0.5, material_right));

        // Render
        for (int j = image_height - 1; j >= 0; --j)
        {
            for (int i = 0; i < image_width; ++i)
            {
                vec3 pixel_color(0, 0, 0);
                for (int s = 0; s < samples; ++s)
                {
                    f32 u = (i + linearRand(0.f, 1.f)) / (image_width - 1);
                    f32 v = (j + linearRand(0.f, 1.f)) / (image_height - 1);
                    Ray r = cam.getRay(u, v);
                    pixel_color += getColor(r, model, max_depth);
                }
                writeColor(pixels, ivec2(image_width, image_height), ivec2(i, j), pixel_color, samples);
            }
        }
    }

    void PathTracer::writeColor(unsigned char *pixels, ivec2 tex_size, ivec2 tex_coord, vec3 color, int samples)
    {
        auto r = color.x;
        auto g = color.y;
        auto b = color.z;

        auto scale = 1.0 / samples;
        r = sqrt(scale * r);
        g = sqrt(scale * g);
        b = sqrt(scale * b);

        pixels[3 * (tex_size.x * tex_coord.y + tex_coord.x) + 0] = static_cast<int>(256 * Math::clamp(r, 0.0, 0.999));
        pixels[3 * (tex_size.x * tex_coord.y + tex_coord.x) + 1] = static_cast<int>(256 * Math::clamp(g, 0.0, 0.999));
        pixels[3 * (tex_size.x * tex_coord.y + tex_coord.x) + 2] = static_cast<int>(256 * Math::clamp(b, 0.0, 0.999));
    }
}