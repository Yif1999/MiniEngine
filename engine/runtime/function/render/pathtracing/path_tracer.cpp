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
    PathTracer::PathTracer()
    {
        init_info.BounceLimit = 4;
        init_info.BVH =true;
        init_info.Denoise = true;
        init_info.MultiThread = true;
        init_info.Output = false;
        init_info.Resolution= glm::ivec2(640, 480);
    }

    vec3 PathTracer::getColor(const Ray &r, const Hittable &mesh, shared_ptr<HittableList> &lights, int depth)
    {
        HitRecord rec;

        if (depth <= 0)
        {
            return vec3(0, 0, 0);
        }

        if (!mesh.hit(r, EPS, INF, rec))
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
            return srec.attenuation * getColor(srec.specular_ray, mesh, lights, depth - 1);
        }

        // auto light_ptr = make_shared<HittablePDF>(lights, rec.hit_point.Position);
        // MixturePDF p(light_ptr, srec.pdf_ptr);


        Ray scattered = Ray(rec.hit_point.Position, srec.pdf_ptr->generate());
        auto pdf = srec.pdf_ptr->value(scattered.direction);

        return emitted + srec.attenuation * rec.mat_ptr->scatterPDF(r, rec, scattered) * getColor(scattered, mesh, lights, depth - 1) / pdf;
    }

    void PathTracer::startRender(unsigned char *pixels)
    {
        int window_size = 512;

        // Image
        const int image_width = window_size;
        const int image_height = window_size;
        const int samples = 128;
        const int max_depth = 8;

        // Camera
        vec3 lookfrom(6.9118194580078125, 1.6516278982162476, 2.5541365146636963);
        vec3 lookat(2.328019380569458, 1.6516276597976685, 0.33640459179878235);
        vec3 vup(0, 1, 0);
        auto dist_to_focus = 10.0;
        auto aperture = 0;

        Camera cam(lookfrom, lookat, vup, 60, aperture, dist_to_focus);

        // Model
        HittableList mesh;
        mesh.add(make_shared<BVH>(mesh_data));

        // Light
        auto lights = make_shared<HittableList>(light_data);

        // Render
        std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();

        // multi thread
        for (int j = image_height - 1; j >= 0; --j)
        {
            tbb::parallel_for(0, image_width, 
            [this, j, samples, image_width, image_height, max_depth, &cam, &mesh, &lights, &pixels]
            (int i){
                vec3 pixel_color(0, 0, 0);
                for (int s = 0; s < samples; ++s)
                {
                    f32 u = (i + linearRand(0.f, 1.f)) / (image_width - 1);
                    f32 v = (j + linearRand(0.f, 1.f)) / (image_height - 1);
                    Ray r = cam.getRay(u, v);
                    vec3 sample_color = getColor(r, mesh, lights, max_depth);
                    if (isInfinity(sample_color) || isNan(sample_color))
                        sample_color={0,0,0};
                    pixel_color += sample_color;
                }
                writeColor(pixels, ivec2(image_width, image_height), ivec2(i, j), pixel_color, samples); });
        }

        std::chrono::steady_clock::time_point endTime = std::chrono::steady_clock::now();
        std::chrono::duration<float> time_span = std::chrono::duration_cast<std::chrono::duration<float>>(endTime - startTime);
        float delta_time = time_span.count();

        LOG_INFO("ray tracing is done in " + to_string(delta_time) + "s");

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
        auto r = color.r;
        auto g = color.g;
        auto b = color.b;

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

        color.r = pow(pixels[3 * (tex_size.x * tex_coord.y + tex_coord.x) + 0] / 255.f, 2.2);
        color.g = pow(pixels[3 * (tex_size.x * tex_coord.y + tex_coord.x) + 1] / 255.f, 2.2);
        color.b = pow(pixels[3 * (tex_size.x * tex_coord.y + tex_coord.x) + 2] / 255.f, 2.2);

        return color;
    }

    void PathTracer::transferModelData(shared_ptr<Model> m_model)
    {
        // clean data buffer
        mesh_data.clear();
        light_data.clear();

        // loop meshes
        for (const auto &mesh : m_model->meshes)
        {
            auto mat = make_shared<Phong>(mesh.material);

            // loop triangles
            for (int id = 0; id < mesh.indices.size(); id += 3)
            {
                vector<Vertex> vertices(3);
                vertices[0] = mesh.vertices[mesh.indices[id]];
                vertices[1] = mesh.vertices[mesh.indices[id + 1]];
                vertices[2] = mesh.vertices[mesh.indices[id + 2]];

                mesh_data.add(make_shared<Triangle>(vertices, mat));


                // if (mat->is_emitted(mat->mat))
                // {
                //     light_data.add(make_shared<Triangle>(vt, shared_ptr<Material>()));
                // }
            }
        }

    }
}