#pragma once

#include "runtime/core/base/macro.h"
#include "runtime/function/render/pathtracing/common/ray.h"
#include "runtime/function/render/pathtracing/common/hittable.h"
#include "runtime/function/render/render_model.h"

#include <glm/glm.hpp>

namespace MiniEngine::PathTracing
{
    class PathTracer
    {
    public:
        void startRender(unsigned char *pixels);
        void transferMeshData(shared_ptr<Model> m_model);

    private:
        HittableList mesh_data;

        void writeColor(unsigned char *pixels, glm::ivec2 tex_size, glm::ivec2 tex_coord, glm::vec3 color, int samples);
        glm::vec3 readColor(unsigned char *pixels, glm::ivec2 tex_size, glm::ivec2 tex_coord);
    };
}