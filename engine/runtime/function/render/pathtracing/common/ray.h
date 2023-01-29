#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

namespace MiniEngine
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
            this->direction=direction;
        }

        glm::vec3 cast(double t) const
        {
            return origin + glm::vec1(t) * direction;
        }

    };

}