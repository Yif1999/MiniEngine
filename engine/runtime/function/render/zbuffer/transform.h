#pragma once

#include "runtime/function/render/render_model.h"

#include <glm/glm.hpp>

namespace MiniEngine
{
    void screen_space_transform(Model *m_model, glm::mat4 &model, glm::mat4 &view, glm::mat4 &projection);
}