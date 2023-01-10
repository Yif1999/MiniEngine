#include "runtime/function/transform/transform.h"

#include <glm/glm.hpp>
#include <iostream>

#define window_size 512

namespace MiniEngine
{
    void screen_space_transform(Model *m_model, glm::mat4 &model, glm::mat4 &view, glm::mat4 &projection)
    {
        for (int m = 0; m < m_model->meshes.size(); m++)
        {
            for (int v = 0; v < m_model->meshes[m].vertices.size(); v++)
            {
                glm::vec4 ndc = projection * view * model * glm::vec4(m_model->meshes[m].vertices[v].Position, 1.0f);
                glm::vec3 ssc = glm::vec3(ndc) / ndc[3];
                ssc[0] = (ssc[0]+1)*0.5*window_size;
                ssc[1] = (ssc[1]+1)*0.5*window_size;
                ssc[2] = (ssc[2]+1)*0.5*window_size;
                m_model->meshes[m].vertices[v].Position = ssc;
            }

        }
    }
}