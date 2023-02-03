#include "runtime/function/render/pathtracing/path_tracer.h"
#include "runtime/function/render/pathtracing/common/util.h"
#include "runtime/function/render/pathtracing/common/sphere.h"
#include "runtime/function/render/pathtracing/common/rectangle.h"
#include "runtime/function/render/pathtracing/common/box.h"
#include "runtime/function/render/pathtracing/common/triangle.h"
#include "runtime/function/render/pathtracing/common/camera.h"
#include "runtime/function/render/pathtracing/common/material.h"
#include "runtime/function/render/pathtracing/common/pdf.h"
#include "thirdparty/oidn/include/OpenImageDenoise/oidn.hpp"

namespace MiniEngine
{

    vec3 getColor(const Ray &r, const Hittable &model, shared_ptr<HittableList> &lights, int depth)
    {
        HitRecord rec;

        if (depth <= 0)
            return vec3(0, 0, 0);

        if (!model.hit(r, EPS, INF, rec))
        {
            return vec3(0, 0, 0);
        }

        ScatterRecord srec;
        vec3 emitted = rec.mat_ptr->emitted(r, rec);
        if (!rec.mat_ptr->scatter(r, rec, srec))
        {
            return emitted;
        }

        if (srec.is_specular)
        {
            return srec.attenuation * getColor(srec.specular_ray, model, lights, depth - 1);
        }

        auto light_ptr = make_shared<HittablePDF>(lights, rec.p);
        MixturePDF p(light_ptr, srec.pdf_ptr);

        Ray scattered = Ray(rec.p, p.generate());
        auto pdf = p.value(scattered.direction);

        return emitted + srec.attenuation * rec.mat_ptr->scatterPDF(r, rec, scattered) * getColor(scattered, model, lights, depth - 1) / pdf;
    }

    HittableList random_ball()
    {
        HittableList world;

        auto ground_material = make_shared<Lambertian>(vec3(0.5, 0.5, 0.5));
        world.add(make_shared<Sphere>(vec3(0, -1000, 0), 1000, ground_material));

        for (int a = -11; a < 11; a++)
        {
            for (int b = -11; b < 11; b++)
            {
                auto choose_mat = linearRand(0.f, 1.f);
                vec3 center(a + 0.9 * linearRand(0.f, 1.f), 0.2, b + 0.9 * linearRand(0.f, 1.f));

                if (length(center - vec3(4, 0.2, 0)) > 0.9)
                {
                    shared_ptr<Material> sphere_material;

                    if (choose_mat < 0.5)
                    {
                        // diffuse
                        auto albedo = vec3(0.5, 0.5, 0.5);
                        sphere_material = make_shared<Lambertian>(albedo);
                        world.add(make_shared<Sphere>(center, 0.2, sphere_material));
                    }
                    else if (choose_mat < 0.75)
                    {
                        // metal
                        auto albedo = vec3(0.5, 0.5, 0.5);
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

        auto material2 = make_shared<Emission>(vec3(4, 4, 4));
        world.add(make_shared<Sphere>(vec3(-4, 1, 0), 1.0, material2));

        auto material3 = make_shared<Metal>(vec3(0.7, 0.6, 0.5), 0.0);
        world.add(make_shared<Sphere>(vec3(4, 1, 0), 1.0, material3));

        return world;
    }

    HittableList cornell_box()
    {
        HittableList objects;

        auto red = make_shared<Lambertian>(vec3(.65, .05, .05));
        auto white = make_shared<Lambertian>(vec3(.73, .73, .73));
        auto green = make_shared<Lambertian>(vec3(.12, .45, .15));
        auto light = make_shared<Emission>(vec3(15, 15, 15));

        objects.add(make_shared<RectangleYZ>(0, 555, 0, 555, 555, green));
        objects.add(make_shared<RectangleYZ>(0, 555, 0, 555, 0, red));
        objects.add(make_shared<RectangleXZ>(213, 343, 227, 332, 554, light));
        objects.add(make_shared<RectangleXZ>(0, 555, 0, 555, 0, white));
        objects.add(make_shared<RectangleXZ>(0, 555, 0, 555, 555, white));
        objects.add(make_shared<RectangleXY>(0, 555, 0, 555, 555, white));
        vector<vec3> triangle = {vec3(80, 190, 110),vec3(34, 22, 200),vec3(170, 10, 190)};
        objects.add(make_shared<Triangle>(triangle,white));

        shared_ptr<Material> aluminum = make_shared<Metal>(vec3(0.8, 0.85, 0.88), 0.0);
        shared_ptr<Hittable> box1 = make_shared<Box>(vec3(0, 0, 0), vec3(165, 330, 165), aluminum);
        box1 = make_shared<RotateY>(box1, 15);
        box1 = make_shared<Translate>(box1, vec3(265, 0, 295));
        objects.add(box1);

        // auto glass = make_shared<Dielectric>(1.5);
        // objects.add(make_shared<Sphere>(vec3(190, 90, 190), 90, glass));

        return objects;
    }

    void PathTracer::startRender(unsigned char *pixels)
    {

        clock_t startTime, endTime;

        int window_size = 512;

        // Image
        const int image_width = window_size;
        const int image_height = window_size;
        const int samples = 10;
        const int max_depth = 10;

        // Camera
        vec3 lookfrom(278, 278, -800);
        vec3 lookat(278, 278, 0);
        vec3 vup(0, 1, 0);
        auto dist_to_focus = 10.0;
        auto aperture = 0;

        Camera cam(lookfrom, lookat, vup, 40, aperture, dist_to_focus);

        // Model
        auto model = cornell_box();
        auto lights = make_shared<HittableList>();
        lights->add(make_shared<RectangleXZ>(213, 343, 227, 332, 554, shared_ptr<Material>()));

        // Render
        startTime = clock();
        for (int j = image_height - 1; j >= 0; --j)
        {
            for (int i = 0; i < image_width; ++i)
            {
                vec3 pixel_color(0, 0, 0);

                // if (i>320 && i<380 && j>150 && j<180)
                {
                for (int s = 0; s < samples; ++s)
                {
                    f32 u = (i + linearRand(0.f, 1.f)) / (image_width - 1);
                    f32 v = (j + linearRand(0.f, 1.f)) / (image_height - 1);
                    Ray r = cam.getRay(u, v);
                    pixel_color += getColor(r, model, lights, max_depth);
                }
                writeColor(pixels, ivec2(image_width, image_height), ivec2(i, j), pixel_color, samples);

                }
            }
        }
        endTime = clock();

        LOG_INFO("ray tracing is done in " + to_string((float)(endTime - startTime) / CLOCKS_PER_SEC) + "s");

        // Denoise
        float *denoise_buffer = new float[3*512*512];

        for (int j = image_height - 1; j >= 0; --j)
        {
            for (int i = 0; i < image_width; ++i)
            {
                vec3 swap_color=readColor(pixels,ivec2(image_width, image_height), ivec2(i, j));
                denoise_buffer[3 * (image_width * j + i) + 0] = swap_color.x;
                denoise_buffer[3 * (image_width * j + i) + 1] = swap_color.y;
                denoise_buffer[3 * (image_width * j + i) + 2] = swap_color.z;
            }
        }

        // Create an Intel Open Image Denoise device
        oidn::DeviceRef device = oidn::newDevice();
        device.commit();

        // Create a filter for denoising a beauty (color) image using optional auxiliary images too
        oidn::FilterRef filter = device.newFilter("RT");                     // generic ray tracing filter
        filter.setImage("color", denoise_buffer, oidn::Format::Float3, 512, 512);   // beauty
        filter.setImage("output", denoise_buffer, oidn::Format::Float3, 512, 512); // denoised beauty
        filter.commit();

        // Filter the image
        filter.execute();

        // Check for errors
        const char *errorMessage;
        if (device.getError(errorMessage) != oidn::Error::None)
            std::cout << "Error: " << errorMessage << std::endl;

        for (int j = image_height - 1; j >= 0; --j)
        {
            for (int i = 0; i < image_width; ++i)
            {
                vec3 swap_color;
                swap_color.x = denoise_buffer[3 * (image_width * j + i) + 0];
                swap_color.y = denoise_buffer[3 * (image_width * j + i) + 1];
                swap_color.z = denoise_buffer[3 * (image_width * j + i) + 2];
                writeColor(pixels, ivec2(image_width, image_height), ivec2(i, j), swap_color, 1);
            }
        }

    }

    void PathTracer::writeColor(unsigned char *pixels, ivec2 tex_size, ivec2 tex_coord, vec3 color, int samples)
    {
        auto r = color.x;
        auto g = color.y;
        auto b = color.z;

        if (r != r)
            r = 0.0;
        if (g != g)
            g = 0.0;
        if (b != b)
            b = 0.0;

        auto scale = 1.0 / samples;
        r = pow(scale * r, 1/2.2);
        g = pow(scale * g, 1/2.2);
        b = pow(scale * b, 1/2.2);

        pixels[3 * (tex_size.x * tex_coord.y + tex_coord.x) + 0] = static_cast<int>(256 * Math::clamp(r, 0.0, 0.999));
        pixels[3 * (tex_size.x * tex_coord.y + tex_coord.x) + 1] = static_cast<int>(256 * Math::clamp(g, 0.0, 0.999));
        pixels[3 * (tex_size.x * tex_coord.y + tex_coord.x) + 2] = static_cast<int>(256 * Math::clamp(b, 0.0, 0.999));
    }

    vec3 PathTracer::readColor(unsigned char *pixels, ivec2 tex_size, ivec2 tex_coord)
    {
        vec3 color;

        color.x = pow(pixels[3 * (tex_size.x * tex_coord.y + tex_coord.x) + 0]/255.f, 2.2) ;
        color.y = pow(pixels[3 * (tex_size.x * tex_coord.y + tex_coord.x) + 1]/255.f, 2.2) ;
        color.z = pow(pixels[3 * (tex_size.x * tex_coord.y + tex_coord.x) + 2]/255.f, 2.2) ;

        return color;
    }
}