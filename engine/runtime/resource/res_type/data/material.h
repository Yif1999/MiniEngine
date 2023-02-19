#pragma once
#include "runtime/core/meta/reflection/reflection.h"

namespace MiniEngine
{
    REFLECTION_TYPE(MaterialData)
    CLASS(MaterialData, Fields)
    {
        REFLECTION_BODY(MaterialData);

    public:
        std::string name;

        Vector3 Kd; // diffuse reflectance of material, map_Kd is the texture file path.
        Vector3 Ks; // specular reflectance of material.
        Vector3 Ke; // emission intensity of light.
        Vector3 Tr; // transmittance of material.
        float Ns;     // shiness, the exponent of phong lobe.
        float Ni;     // Index of Refraction(IOR) of transparent object like glass and water.

        std::string map_Kd;
    };
} // namespace MiniEngine