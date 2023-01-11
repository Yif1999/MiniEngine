#include "runtime/function/transform/transform.h"

#include <glm/glm.hpp>
#include <iostream>

#define window_size 512

namespace MiniEngine
{
    void screen_space_transform(Mesh *mesh, glm::mat4 &model, glm::mat4 &view, glm::mat4 &projection)
    {
        for (int v = 0; v < mesh->vertices.size(); v++)
        {
            glm::vec4 ndc = projection * view * model * glm::vec4(mesh->vertices[v].Position, 1.0f);
            glm::vec3 ssc = glm::vec3(ndc) / ndc[3];
            ssc[0] = (ssc[0]+1)*0.5*window_size;
            ssc[1] = (ssc[1]+1)*0.5*window_size;
            ssc[2] = (ssc[2]+1)*0.5*window_size;
            mesh->vertices[v].Position = ssc;
        }

    }

    void screen_space_transform(OctTree::Bound *bound, glm::mat4 &model, glm::mat4 &view, glm::mat4 &projection)
    {
        for (int i=0;i<8;i++)
        {
            glm::vec4 ndc = projection * view * model * glm::vec4(bound->corner[i], 1.0f);
            glm::vec3 ssc = glm::vec3(ndc) / ndc[3];
            ssc[0] = (ssc[0]+1)*0.5*window_size;
            ssc[1] = (ssc[1]+1)*0.5*window_size;
            ssc[2] = (ssc[2]+1)*0.5*window_size;
            bound->corner[i] = ssc;
        }
    }
}