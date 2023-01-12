#pragma once

#include "runtime/function/render/render_model.h"
#include "runtime/resource/acc_struct/octree.h"

#include <glm/glm.hpp>

namespace MiniEngine
{
    void screen_space_transform(Mesh *mesh, glm::mat4 &model, glm::mat4 &view, glm::mat4 &projection);

    void screen_space_transform(OctTree::Bound *bound, glm::mat4 &model, glm::mat4 &view, glm::mat4 &projection);

}