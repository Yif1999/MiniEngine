#pragma once

#include "runtime/function/render/pathtracing/util.h"

namespace MiniEngine
{
    class Camera
    {
    public:
        Camera(vec3 lookfrom,
               vec3 lookat,
               vec3 up,
               float fov,
               float aperture,
               float focus_dist)
        {
            auto theta = radians(fov);
            auto aspect_ratio = 1.0;
            auto h = tan(theta / 2);
            f32 viewport_height = 2.0 * h;
            f32 viewport_width = aspect_ratio * viewport_height;
            auto focal_length = 1.0;

            w = normalize(lookfrom - lookat);
            u = normalize(cross(up, w));
            v = cross(w, u);

            origin = lookfrom;
            horizontal = focus_dist * viewport_width * u;
            vertical = focus_dist * viewport_height * v;
            lower_left_corner = origin - horizontal / f32(2) - vertical / f32(2) - focus_dist * w;

            lens_radius = aperture / f32(2);
        }

        Ray getRay(f32 s, f32 t) const
        {
            vec3 rd = lens_radius * vec3(diskRand(1.f), 0);
            vec3 offset = u * rd.x + v * rd.y;

            return Ray(origin + offset, lower_left_corner + s * horizontal + t * vertical - origin - offset);
        }

    private:
        vec3 origin;
        vec3 lower_left_corner;
        vec3 horizontal;
        vec3 vertical;
        vec3 u, v, w;
        float lens_radius;
    };
}