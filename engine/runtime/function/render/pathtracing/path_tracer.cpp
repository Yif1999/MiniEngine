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

#define MaxLights 8

namespace MiniEngine::PathTracing
{
    PathTracer::PathTracer()
    {
        init_info = make_shared<RenderingInitInfo>();
        init_info->BounceLimit = 4;
        init_info->ImportSample = true;
        init_info->BVH = true;
        init_info->Denoise = true;
        init_info->MultiThread = true;
        init_info->Output = false;
        init_info->Resolution = glm::ivec2(1280, 720);
        init_info->SampleCount = 128;
    }

    void PathTracer::initializeRenderer()
    {
        width = init_info->Resolution.x;
        height = init_info->Resolution.y;

        if (pixels)
        {
            delete pixels;
            pixels = nullptr;
        }
        pixels = new unsigned char[3 * width * height];
        memset(pixels, 0, sizeof(char) * width * height * 3);

        if (result)
        {
            glDeleteTextures(1, &result);
        }
        glGenTextures(1, &result);
        glBindTexture(GL_TEXTURE_2D, result);
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    }

    vec3 PathTracer::getColor(const Ray &r, const Hittable &mesh, shared_ptr<HittableList> &lights, int depth, bool importance_sampling)
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
            return srec.attenuation * getColor(srec.specular_ray, mesh, lights, depth - 1, importance_sampling);
        }

        Ray scattered;
        float pdf;
        if (importance_sampling)
        {
            auto light_ptr = make_shared<HittablePDF>(lights, rec.hit_point.Position);
            MixturePDF p(light_ptr, srec.pdf_ptr, 0.5);

            scattered = Ray(rec.hit_point.Position, p.generate());
            pdf = p.value(scattered.direction);
        }
        else
        {
            scattered = Ray(rec.hit_point.Position, srec.pdf_ptr->generate());
            pdf = srec.pdf_ptr->value(scattered.direction);
        }

        return emitted + srec.attenuation * rec.mat_ptr->scatterPDF(r, rec, scattered) * getColor(scattered, mesh, lights, depth - 1, importance_sampling) / pdf;
    }

    void PathTracer::startTracing(shared_ptr<Model> m_model, shared_ptr<MiniEngine::Camera> m_camera)
    {
        state = 0;
        std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();

        transferModelData(m_model);

        // Image
        const int samples = init_info->SampleCount;
        const int max_depth = init_info->BounceLimit;
        const bool importance_sampling = init_info->ImportSample;

        // Light
        if (!getMainLightNumber()){
            return;
        }
        auto lights = make_shared<HittableList>(light_data);

        // Model
        HittableList mesh;
        if (init_info->BVH)
        {
            state = 1;
            mesh.add(make_shared<BVH>(mesh_data));
        }
        else
        {
            mesh = mesh_data;
        }

        // Camera
        vec3 direction(cos(glm::radians(m_camera->Yaw))*cos(glm::radians(m_camera->Pitch)),sin(glm::radians(m_camera->Pitch)),sin(glm::radians(m_camera->Yaw))*cos(glm::radians(m_camera->Pitch)));
        vec3 lookfrom(m_camera->Position);
        vec3 lookat(m_camera->Position + direction);
        vec3 vup(0, 1, 0);
        auto aperture = m_camera->Aperture;
        auto fov = m_camera->Zoom;
        auto aspect_ratio = (float)init_info->Resolution.x/(float)init_info->Resolution.y;

        Camera laser(lookfrom, lookat, vup, fov, 0.f, 1.f, aspect_ratio);
                
        f32 dist_to_focus;
        if (!m_camera->FocusMode)
        { 
            Ray r = laser.getRay(0.5f, 0.5f);
            HitRecord rec;
            mesh.hit(r, EPS, INF, rec);
            dist_to_focus = rec.t;
        }
        else
        {
            dist_to_focus = m_camera->FocusDistance;
        }

        Camera cam(lookfrom, lookat, vup, fov, aperture, dist_to_focus, aspect_ratio);

        state = 2;
        // Render
        for (int j = height - 1; j >= 0; --j)
        {
            progress = Math::clamp(float(height-j) / float(height-1) * 100, 0, 100);

            if (init_info->MultiThread)
            {
                // multi thread
                tbb::parallel_for(0, width,
                                [this, j, samples, max_depth, importance_sampling , &cam, &mesh, &lights](int i)
                                {
                    vec3 pixel_color(0, 0, 0);
                    for (int s = 0; s < samples; ++s)
                    {
                        if (should_stop_tracing)
                            return;

                        f32 u = (i + linearRand(0.f, 1.f)) / (width - 1);
                        f32 v = (j + linearRand(0.f, 1.f)) / (height - 1);
                        Ray r = cam.getRay(u, v);
                        vec3 sample_color = getColor(r, mesh, lights, max_depth, importance_sampling);
                        if (isInfinity(sample_color) || isNan(sample_color))
                            sample_color={0,0,0};
                        pixel_color += sample_color;
                    }
                    pixel_color *= 1.0 / samples;
                    writeColor(pixels, ivec2(width, height), ivec2(i, j), pixel_color, 2.2); });
                if (should_stop_tracing)
                    return;
            }
            else
            {
                // single thread
                for (int i = 0; i < width; ++i)
                {
                    vec3 pixel_color(0, 0, 0);
                    for (int s = 0; s < samples; ++s)
                    {
                        if (should_stop_tracing)
                            return;

                        f32 u = (i + linearRand(0.f, 1.f)) / (width - 1);
                        f32 v = (j + linearRand(0.f, 1.f)) / (height - 1);
                        Ray r = cam.getRay(u, v);
                        vec3 sample_color = getColor(r, mesh, lights, max_depth, importance_sampling);
                        if (isInfinity(sample_color) || isNan(sample_color))
                            sample_color={0,0,0};
                        pixel_color += sample_color;
                    }
                    pixel_color *= 1.0 / samples;
                    writeColor(pixels, ivec2(width, height), ivec2(i, j), pixel_color, 2.2);
                }
            }
        }


        if (init_info->Denoise)
        {
            state = 3;
            // Denoise
            float *denoise_buffer = new float[3 * width * height];

            for (int j = height - 1; j >= 0; --j)
            {
                for (int i = 0; i < width; ++i)
                {
                    vec3 swap_color = readColor(pixels, ivec2(width, height), ivec2(i, j), 2.2);
                    denoise_buffer[3 * (width * j + i) + 0] = swap_color.x;
                    denoise_buffer[3 * (width * j + i) + 1] = swap_color.y;
                    denoise_buffer[3 * (width * j + i) + 2] = swap_color.z;
                }
            }

            // Create an Intel Open Image Denoise device
            oidn::DeviceRef device = oidn::newDevice();
            device.commit();

            // Create a filter for denoising a beauty (color) image using optional auxiliary images too
            oidn::FilterRef filter = device.newFilter("RT");                           // generic ray tracing filter
            filter.setImage("color", denoise_buffer, oidn::Format::Float3, width, height);  // beauty
            filter.setImage("output", denoise_buffer, oidn::Format::Float3, width, height); // denoised beauty
            filter.set("hdr", false);
            filter.commit();

            // Filter the image
            filter.execute();

            // Check for errors
            const char *errorMessage;
            if (device.getError(errorMessage) != oidn::Error::None)
                std::cout << "Error: " << errorMessage << std::endl;

            for (int j = height - 1; j >= 0; --j)
            {
                for (int i = 0; i < width; ++i)
                {
                    vec3 swap_color;
                    swap_color.x = denoise_buffer[3 * (width * j + i) + 0];
                    swap_color.y = denoise_buffer[3 * (width * j + i) + 1];
                    swap_color.z = denoise_buffer[3 * (width * j + i) + 2];
                    writeColor(pixels, ivec2(width, height), ivec2(i, j), swap_color, 2.2);
                }
            }

            free(denoise_buffer);
        }

        if (init_info->Output)
        {
            // unsigned char *output_buffer = new unsigned char[3 * width * height];

            // for (int j = height - 1; j >= 0; --j)
            // {
            //     for (int i = 0; i < width; ++i)
            //     {
            //         vec3 srgb_color = readColor(pixels, ivec2(width, height), ivec2(i, j), 1.0);
            //         writeColor(output_buffer, ivec2(width, height), ivec2(i, j), srgb_color, 2.2);
            //     }
            // }

            stbi_flip_vertically_on_write(true);
            stbi_write_png(init_info->SavePath, width, height, 3, pixels, 0);

            // free(output_buffer);
        }

        state = 4;

        std::chrono::steady_clock::time_point endTime = std::chrono::steady_clock::now();
        std::chrono::duration<float> time_span = std::chrono::duration_cast<std::chrono::duration<float>>(endTime - startTime);
        render_time = time_span.count();

    }

    void PathTracer::writeColor(unsigned char *pixels, ivec2 tex_size, ivec2 tex_coord, vec3 color, float gama)
    {
        auto r = color.r;
        auto g = color.g;
        auto b = color.b;

        r = pow(r, 1 / gama);
        g = pow(g, 1 / gama);
        b = pow(b, 1 / gama);

        pixels[3 * (tex_size.x * tex_coord.y + tex_coord.x) + 0] = static_cast<int>(256 * Math::clamp(r, 0.0, 0.999));
        pixels[3 * (tex_size.x * tex_coord.y + tex_coord.x) + 1] = static_cast<int>(256 * Math::clamp(g, 0.0, 0.999));
        pixels[3 * (tex_size.x * tex_coord.y + tex_coord.x) + 2] = static_cast<int>(256 * Math::clamp(b, 0.0, 0.999));
    }

    vec3 PathTracer::readColor(unsigned char *pixels, ivec2 tex_size, ivec2 tex_coord, float gama)
    {
        vec3 color;

        color.r = pow(pixels[3 * (tex_size.x * tex_coord.y + tex_coord.x) + 0] / 255.f, gama);
        color.g = pow(pixels[3 * (tex_size.x * tex_coord.y + tex_coord.x) + 1] / 255.f, gama);
        color.b = pow(pixels[3 * (tex_size.x * tex_coord.y + tex_coord.x) + 2] / 255.f, gama);

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
            auto mat = make_shared<Phong>(mesh.material, m_model->model_path);

            // loop triangles
            for (int id = 0; id < mesh.indices.size(); id += 3)
            {
                vector<Vertex> vertices(3);
                vertices[0] = mesh.vertices[mesh.indices[id]];
                vertices[1] = mesh.vertices[mesh.indices[id + 1]];
                vertices[2] = mesh.vertices[mesh.indices[id + 2]];

                mesh_data.add(make_shared<Triangle>(vertices, mat));

                if (mat->is_emitted(mat->mat))
                {
                    light_data.add(make_shared<Triangle>(vertices, shared_ptr<Material>()));
                }
            }
        }

        // get main light (for importance sampling)
        std::map<shared_ptr<Hittable>, float> light_with_area;
        for (const auto &light : light_data.objects)
        {
            light_with_area.insert(pair<shared_ptr<Hittable>, float>(light, light->getArea()));
        }
        
        vector<pair<shared_ptr<Hittable>, float>> map_vec;
        for(map<shared_ptr<Hittable>, float>::iterator it = light_with_area.begin(); it != light_with_area.end(); it++)
        {
            map_vec.push_back( pair<shared_ptr<Hittable>, float>(it->first,it->second) );
        }
        sort(map_vec.begin(),map_vec.end(),hittableCompare);
        light_data.clear();
        for (const auto &light_map : map_vec)
        {
            light_data.objects.push_back(light_map.first);            
            if (light_data.objects.size() == MaxLights)
                break;
        }
    }

    int PathTracer::getMainLightNumber()
    {
        return light_data.objects.size();
    }
}