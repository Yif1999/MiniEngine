#pragma once

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "runtime/core/meta/reflection/reflection.h"
#include "runtime/resource/res_type/data/material.h"

#include <string>
#include <vector>

namespace MiniEngine
{
    REFLECTION_TYPE(VertexData)
    CLASS(VertexData, Fields)
    {
        REFLECTION_BODY(VertexData);

    public:
        float px;
        float py;
        float pz;
        float nx;
        float ny;
        float nz;
        float tx;
        float ty;
        float tz;
        float u;
        float v;
    };

    REFLECTION_TYPE(MeshData)
    CLASS(MeshData, Fields)
    {
        REFLECTION_BODY(MeshData);

    public:
        // mesh Data
        std::vector<VertexData> vertices;
        std::vector<int> indices;

    };
}
