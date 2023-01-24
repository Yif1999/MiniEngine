#include "runtime/function/render/pathtracing/path_tracer.h"
#include "runtime/function/render/pathtracing/util.h"
#include "runtime/function/render/pathtracing/sphere.h"
#include "runtime/function/render/pathtracing/camera.h"

namespace MiniEngine
{

    vec3 getColor(const Ray &r, const Hittable &model, int depth)
    {
        HitRecord rec;

        if (depth <= 0)
            return vec3(0, 0, 0);

        if (model.hit(r, 0.001, INF, rec))
        {
            vec3 target = rec.p + rec.normal + sphericalRand(1.f);
            return f32(0.5) * getColor(Ray(rec.p, target - rec.p), model, depth - 1);
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

        // World
        HittableList model;
        model.add(make_shared<Sphere>(vec3(0, 0, -1), 0.5));
        model.add(make_shared<Sphere>(vec3(0, -100.5, -1), 100));

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