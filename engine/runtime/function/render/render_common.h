#pragma once

#include "runtime/core/math/matrix4.h"
#include "runtime/core/math/vector3.h"
#include "runtime/core/math/vector4.h"
#include "runtime/function/render/render_type.h"

namespace MiniEngine
{
    struct MeshPerframeStorageBufferObject
    {
        Matrix4x4                   proj_view_matrix;
        Vector3                     camera_position;
        float                       _padding_camera_position;
        Vector3                     ambient_light;
        float                       _padding_ambient_light;
        uint32_t                    point_light_num;
        uint32_t                    _padding_point_light_num_1;
        uint32_t                    _padding_point_light_num_2;
        uint32_t                    _padding_point_light_num_3;
        Matrix4x4                   directional_light_proj_view;
    };

    struct MeshInefficientPickPerframeStorageBufferObject
    {
        Matrix4x4 proj_view_matrix;
        uint32_t  rt_width;
        uint32_t  rt_height;
    };

}
