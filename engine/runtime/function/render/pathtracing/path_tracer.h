#pragma once

#include "runtime/core/base/macro.h"
#include "runtime/function/render/pathtracing/common/ray.h"
#include "runtime/function/render/pathtracing/common/hittable.h"
#include "runtime/function/render/pathtracing/common/material.h"
#include "runtime/function/render/render_model.h"

#include <glm/glm.hpp>

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
        unsigned int RT;
        bool should_stop_tracing{false};
        unsigned char *pixels = nullptr;
        shared_ptr<RenderingInitInfo> init_info;

        PathTracer();

        void initializeRenderer();
        void startTracing(shared_ptr<Model> m_model);
        void transferModelData(shared_ptr<Model> m_model);

    private:
        HittableList mesh_data;
        HittableList light_data;

        glm::vec3 getColor(const Ray &r, const Hittable &model, shared_ptr<HittableList> &lights, int depth);
        void writeColor(unsigned char *pixels, glm::ivec2 tex_size, glm::ivec2 tex_coord, glm::vec3 color, int samples);
        glm::vec3 readColor(unsigned char *pixels, glm::ivec2 tex_size, glm::ivec2 tex_coord);
    };
}