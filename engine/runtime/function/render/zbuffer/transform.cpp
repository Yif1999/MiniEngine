#include "runtime/function/render/zbuffer/transform.h"

#include <glm/glm.hpp>
#include <iostream>

namespace MiniEngine
{
    void transform(std::shared_ptr<Model> m_model, glm::mat4 &model, glm::mat4 &view, glm::mat4 &projection)
    {
        for (int m = 0; m < m_model->meshes.size(); m++)
        {
            for (int v = 0; v < m_model->meshes[m].vertices.size(); v++)
            {
                glm::vec4 ndc = projection * view * model * glm::vec4(m_model->meshes[m].vertices[v].Position, 1.0f);
                m_model->meshes[m].vertices[v].Position = glm::vec3(ndc) / ndc[3];
            }

        }
        return;
    }
}