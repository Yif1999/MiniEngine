#pragma once

#include "runtime/core/base/macro.h"
#include "runtime/function/render/pathtracing/common/ray.h"
#include "runtime/function/render/pathtracing/common/hittable.h"
#include "runtime/function/render/pathtracing/common/material.h"
#include "runtime/function/render/render_model.h"
#include "runtime/function/render/render_camera.h"

#include <glm/glm.hpp>
#include <stb_image_write.h>

namespace MiniEngine::PathTracing
{
    struct RenderingInitInfo
    {
        ivec2 Resolution;
        int SampleCount;
        int BounceLimit;
        bool BVH;
        bool MultiThread;
        bool Denoise;
        bool Output;
        char SavePath[128];
    };

    class PathTracer
    {
    public:
        int width;
        int height;
        unsigned int result;
        bool should_stop_tracing{false};
        unsigned char *pixels = nullptr;
        shared_ptr<RenderingInitInfo> init_info;

        PathTracer();

        void initializeRenderer();
        void startTracing(shared_ptr<Model> m_model, shared_ptr<MiniEngine::Camera> m_camera);
        void transferModelData(shared_ptr<Model> m_model);

    private:
        HittableList mesh_data;
        HittableList light_data;

        glm::vec3 getColor(const Ray &r, const Hittable &model, shared_ptr<HittableList> &lights, int depth);
        void writeColor(unsigned char *pixels, glm::ivec2 tex_size, glm::ivec2 tex_coord, glm::vec3 color, float gama);
        glm::vec3 readColor(unsigned char *pixels, glm::ivec2 tex_size, glm::ivec2 tex_coord, float gama);
    };
}