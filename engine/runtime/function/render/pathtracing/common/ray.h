#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

namespace MiniEngine::PathTracing
{
    class Ray
    {
    public:
        glm::vec3 origin;
        glm::vec3 direction;

        Ray() {}
        Ray(const glm::vec3 &origin, const glm::vec3 &direction)
        {
            this->origin=origin;
            this->direction=glm::normalize(direction);
        }

        glm::vec3 cast(float t) const
        {
            return origin + glm::f32(t) * direction;
        }

    };

}