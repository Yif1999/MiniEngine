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
        vec3 unit_direction = normalize(r.direction);
        auto t = 0.5 * (unit_direction.y + 1.0);
        return f32(1.0 - t) * vec3(1.0, 1.0, 1.0) + f32(t) * vec3(0.5, 0.7, 1.0);
    }

    HittableList random_scene()
    {
        HittableList world;

        auto ground_material = make_shared<Lambertian>(vec3(0.5, 0.5, 0.5));
        world.add(make_shared<Sphere>(vec3(0, -1000, 0), 1000, ground_material));

        for (int a = -11; a < 11; a++)
        {
            for (int b = -11; b < 11; b++)
            {
                auto choose_mat = linearRand(0.f,1.f);
                vec3 center(a + 0.9 * linearRand(0.f,1.f), 0.2, b + 0.9 * linearRand(0.f,1.f));

                if (length(center - vec3(4, 0.2, 0)) > 0.9)
                {
                    shared_ptr<Material> sphere_material;

                    if (choose_mat < 0.5)
                    {
                        // diffuse
                        auto albedo = vec3(0.5,0.5,0.5);
                        sphere_material = make_shared<Lambertian>(albedo);
                        world.add(make_shared<Sphere>(center, 0.2, sphere_material));
                    }
                    else if (choose_mat < 0.75)
                    {
                        // metal
                        auto albedo = vec3(0.5, 0.5,0.5);
                        auto fuzz = linearRand(0.f, 0.2f);
                        sphere_material = make_shared<Metal>(albedo, fuzz);
                        world.add(make_shared<Sphere>(center, 0.2, sphere_material));
                    }
                    else
                    {
                        // glass
                        sphere_material = make_shared<Dielectric>(1.5);
                        world.add(make_shared<Sphere>(center, 0.2, sphere_material));
                    }
                }
            }
        }

        auto material1 = make_shared<Dielectric>(1.5);
        world.add(make_shared<Sphere>(vec3(0, 1, 0), 1.0, material1));

        auto material2 = make_shared<Lambertian>(vec3(0.4, 0.2, 0.1));
        world.add(make_shared<Sphere>(vec3(-4, 1, 0), 1.0, material2));

        auto material3 = make_shared<Metal>(vec3(0.7, 0.6, 0.5), 0.0);
        world.add(make_shared<Sphere>(vec3(4, 1, 0), 1.0, material3));

        return world;
    }

    void PathTracer::startRender(unsigned char *pixels)
    {
        int window_size = 512;

        // Image
        const int image_width = window_size;
        const int image_height = window_size;
        const int samples = 100;
        const int max_depth = 10;

        // Camera
        vec3 lookfrom(13, 2, 3);
        vec3 lookat(0, 0, 0);
        vec3 vup(0, 1, 0);
        auto dist_to_focus = 10.0;
        auto aperture = 0.1;

        Camera cam(lookfrom, lookat, vup, 20, aperture, dist_to_focus);

        // Model
        auto model = random_scene();

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