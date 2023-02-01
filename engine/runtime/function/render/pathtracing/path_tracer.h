#pragma once

#include "runtime/core/base/macro.h"
#include "runtime/function/render/pathtracing/common/ray.h"

#include <glm/glm.hpp>

namespace MiniEngine
{
    class PathTracer
    {
    public:
        void startRender(unsigned char *pixels);

    private:
        void writeColor(unsigned char *pixels, glm::ivec2 tex_size, glm::ivec2 tex_coord, glm::vec3 color, int samples);
        glm::vec3 readColor(unsigned char *pixels, glm::ivec2 tex_size, glm::ivec2 tex_coord);
    };
}