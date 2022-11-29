#pragma once

#include <glm/gtx/hash.hpp>

template<> struct hash<Vertex> {
    size_t operator()(Vertex const& vertex) const {
        return ((hash<glm::vec3>()(vertex.Position) ^
            (hash<glm::vec2>()(vertex.TexCoords) << 1)) >> 1);
    }
};