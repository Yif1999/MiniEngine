#include "runtime/function/render/pathtracing/path_tracer.h"
#include "runtime/function/render/pathtracing/common/util.h"
#include "runtime/function/render/pathtracing/acc_struct/bvh.h"
#include "runtime/function/render/pathtracing/primitive/sphere.h"
#include "runtime/function/render/pathtracing/primitive/rectangle.h"
#include "runtime/function/render/pathtracing/primitive/box.h"
#include "runtime/function/render/pathtracing/primitive/triangle.h"
#include "runtime/function/render/pathtracing/common/camera.h"
#include "runtime/function/render/pathtracing/common/material.h"
#include "runtime/function/render/pathtracing/common/pdf.h"
#include "thirdparty/oidn/include/OpenImageDenoise/oidn.hpp"
#include "thirdparty/tbb/include/tbb/parallel_for.h"


namespace MiniEngine::PathTracing
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
        MixturePDF p(srec.pdf_ptr, srec.pdf_ptr);

        Ray scattered = Ray(rec.p, p.generate());
        auto pdf = p.value(scattered.direction);

        return emitted + srec.attenuation * rec.mat_ptr->scatterPDF(r, rec, scattered) * getColor(scattered, model, lights, depth - 1) / pdf;
    }

    void PathTracer::startRender(unsigned char *pixels)
    {
        clock_t startTime, endTime;

        int window_size = 512;

        // Image
        const int image_width = window_size;
        const int image_height = window_size;
        const int samples = 1000;
        const int max_depth = 10;

        // Camera
        vec3 lookfrom(28.2792, 5.2, 0);
        vec3 lookat(0, 2.8, 0);
        vec3 vup(0, 1, 0);
        auto dist_to_focus = 10.0;
        auto aperture = 0;

        Camera cam(lookfrom, lookat, vup, 30, aperture, dist_to_focus);

        // Model
        HittableList model;
        model.add(make_shared<BVH>(mesh_data));
        auto lights = make_shared<HittableList>();
        lights->add(make_shared<RectangleXZ>(-10, 10, -10, 10, 50, shared_ptr<Material>()));

        // Render
        startTime = clock();
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
                        pixel_color += getColor(r, model, lights, max_depth);
                    }
                    writeColor(pixels, ivec2(image_width, image_height), ivec2(i, j), pixel_color, samples);
            }
        }
        // for (int j = image_height - 1; j >= 0; --j)
        // {
        //     for (int i = 0; i < image_width; ++i)
        //     {
        //         vec3 pixel_color(0, 0, 0);

        //             tbb::parallel_for(0,samples,[](int s){
        //                 f32 u = (i + linearRand(0.f, 1.f)) / (image_width - 1);
        //                 f32 v = (j + linearRand(0.f, 1.f)) / (image_height - 1);
        //                 Ray r = cam.getRay(u, v);
        //                 pixel_color += getColor(r, model, lights, max_depth);
        //             });

        //             writeColor(pixels, ivec2(image_width, image_height), ivec2(i, j), pixel_color, samples);
        //     }
        // }
        // tbb::parallel_for(1, 20000000, [](int i){std::cout << i << std::endl; });
        endTime = clock();

        LOG_INFO("ray tracing is done in " + to_string((float)(endTime - startTime) / CLOCKS_PER_SEC) + "s");

        // Denoise
        float *denoise_buffer = new float[3 * 512 * 512];

        for (int j = image_height - 1; j >= 0; --j)
        {
            for (int i = 0; i < image_width; ++i)
            {
                vec3 swap_color = readColor(pixels, ivec2(image_width, image_height), ivec2(i, j));
                denoise_buffer[3 * (image_width * j + i) + 0] = swap_color.x;
                denoise_buffer[3 * (image_width * j + i) + 1] = swap_color.y;
                denoise_buffer[3 * (image_width * j + i) + 2] = swap_color.z;
            }
        }

        // Create an Intel Open Image Denoise device
        oidn::DeviceRef device = oidn::newDevice();
        device.commit();

        // Create a filter for denoising a beauty (color) image using optional auxiliary images too
        oidn::FilterRef filter = device.newFilter("RT");                           // generic ray tracing filter
        filter.setImage("color", denoise_buffer, oidn::Format::Float3, 512, 512);  // beauty
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
        r = pow(scale * r, 1 / 2.2);
        g = pow(scale * g, 1 / 2.2);
        b = pow(scale * b, 1 / 2.2);

        pixels[3 * (tex_size.x * tex_coord.y + tex_coord.x) + 0] = static_cast<int>(256 * Math::clamp(r, 0.0, 0.999));
        pixels[3 * (tex_size.x * tex_coord.y + tex_coord.x) + 1] = static_cast<int>(256 * Math::clamp(g, 0.0, 0.999));
        pixels[3 * (tex_size.x * tex_coord.y + tex_coord.x) + 2] = static_cast<int>(256 * Math::clamp(b, 0.0, 0.999));
    }

    vec3 PathTracer::readColor(unsigned char *pixels, ivec2 tex_size, ivec2 tex_coord)
    {
        vec3 color;

        color.x = pow(pixels[3 * (tex_size.x * tex_coord.y + tex_coord.x) + 0] / 255.f, 2.2);
        color.y = pow(pixels[3 * (tex_size.x * tex_coord.y + tex_coord.x) + 1] / 255.f, 2.2);
        color.z = pow(pixels[3 * (tex_size.x * tex_coord.y + tex_coord.x) + 2] / 255.f, 2.2);

        return color;
    }

    void PathTracer::transferModelData(shared_ptr<Model> m_model)
    {
        // clean mesh data buffer
        mesh_data.clear();

        // loop meshes
        for (const auto &mesh : m_model->meshes)
        {
            // loop faces
            for (int id = 0; id < mesh.indices.size(); id += 3)
            {
                vector<Vertex> vertices(3);
                vertices[0] = mesh.vertices[mesh.indices[id]];
                vertices[1] = mesh.vertices[mesh.indices[id + 1]];
                vertices[2] = mesh.vertices[mesh.indices[id + 2]];

                vector<vec3> vt(3);
                vt[0]=vertices[0].Position;
                vt[1]=vertices[1].Position;
                vt[2]=vertices[2].Position;

                mesh_data.add(make_shared<Triangle>(vt,make_shared<Phong>(mesh.material)));
            }
        }
    }
}