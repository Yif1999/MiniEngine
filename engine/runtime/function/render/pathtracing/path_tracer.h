#pragma once

#include "runtime/core/base/macro.h"
#include "runtime/function/render/pathtracing/common/ray.h"
#include "runtime/function/render/pathtracing/common/hittable.h"
#include "runtime/function/render/pathtracing/common/material.h"
#include "runtime/function/render/render_model.h"

#include <glm/glm.hpp>
#include <sys/time.h>

namespace MiniEngine::PathTracing
{
    class PathTracer
    {
    public:
        void startRender(unsigned char *pixels);
        void transferModelData(shared_ptr<Model> m_model);

    private:
        HittableList mesh_data;
        HittableList light_data;

        glm::vec3 getColor(const Ray &r, const Hittable &model, shared_ptr<HittableList> &lights, int depth);
        void writeColor(unsigned char *pixels, glm::ivec2 tex_size, glm::ivec2 tex_coord, glm::vec3 color, int samples);
        glm::vec3 readColor(unsigned char *pixels, glm::ivec2 tex_size, glm::ivec2 tex_coord);
    };
}