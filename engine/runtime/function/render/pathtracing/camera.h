#pragma once

#include "runtime/function/render/pathtracing/util.h"

namespace MiniEngine
{
    class Camera
    {
    public:
        Camera()
        {
            auto aspect_ratio = 1.0;
            auto viewport_height = 2.0;
            auto viewport_width = aspect_ratio * viewport_height;
            auto focal_length = 1.0;

            origin = vec3(0, 0, 0);
            horizontal = vec3(viewport_width, 0.0, 0.0);
            vertical = vec3(0.0, viewport_height, 0.0);
            lower_left_corner = origin - horizontal / f32(2) - vertical / f32(2) - vec3(0, 0, focal_length);
        }

        Ray getRay(f32 u, f32 v) const
        {
            return Ray(origin, lower_left_corner + u * horizontal + v * vertical - origin);
        }

    private:
        vec3 origin;
        vec3 lower_left_corner;
        vec3 horizontal;
        vec3 vertical;
    };
}