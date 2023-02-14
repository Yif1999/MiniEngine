#pragma once

#include "runtime/core/meta/reflection/reflection.h"
#include <string>
#include <vector>

namespace MiniEngine
{
    REFLECTION_TYPE(WorldRes)
    CLASS(WorldRes, Fields)
    {
        REFLECTION_BODY(WorldRes);

    public:
        // world name
        std::string              m_name;

        // all scene urls for this world
        std::vector<std::string> m_scene_urls;

        // the default scene for this world, which should be first loading scene
        std::string m_default_scene_url;
    };
} // namespace MiniEngine
