#pragma once

#include <glm/gtx/hash.hpp>

template<> struct std::hash<MiniEngine::Vertex> {
    size_t operator()(MiniEngine::Vertex const& vertex) const {
        return ((hash<glm::vec3>()(vertex.Position) ^
                (hash<glm::vec2>()(vertex.Texcoord) << 1)) >> 1);
    }
};