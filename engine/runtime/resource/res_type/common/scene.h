#pragma once

#include "runtime/core/math/vector3.h"
#include "runtime/core/meta/reflection/reflection.h"
#include "runtime/resource/res_type/common/object.h"

namespace MiniEngine
{
    REFLECTION_TYPE(SceneRes)
    CLASS(SceneRes, Fields)
    {
        REFLECTION_BODY(SceneRes);

    public:
        std::vector<ObjectInstanceRes> m_objects;
    };
} // namespace MiniEngine