#include "runtime/function/render/pathtracing/path_tracer.h"

#include <iostream>
#include <unistd.h>

namespace MiniEngine
{

    float hit_sphere(const glm::vec3 &center, double radius, const Ray &r)
    {
        glm::vec3 oc = r.origin - center;
        auto a = glm::dot(r.direction, r.direction);
        auto b = 2.0 * glm::dot(oc, r.direction);
        auto c = dot(oc, oc) - radius * radius;
        auto discriminant = b * b - 4 * a * c;
        if (discriminant < 0)
        {
            return -1.0;
        }
        else
        {
            return (-b - sqrt(discriminant)) / (2.0 * a);
        }
    }

    glm::vec3 get_color(const Ray &r)
    {
        auto t = hit_sphere(glm::vec3(0, 0, -1), 0.5, r);
        if (t > 0.0)
        {
            glm::vec3 N = glm::normalize(r.cast(t) - glm::vec3(0, 0, -1));
            return glm::f32(0.5) * glm::vec3(N.x + 1, N.y + 1, N.z + 1);
        }
        glm::vec3 unit_direction = r.direction;
        glm::normalize(unit_direction);
        t = 0.5*(unit_direction.y + 1.0);
        return (glm::vec1(1.0 - t) * glm::vec3(1.0, 1.0, 1.0) + glm::vec1(t) * glm::vec3(0.5, 0.7, 1.0));
    }

    void PathTracer::startRender(unsigned char *pixels)
    {
        int window_size = 512;

        // Image
        const int image_width = window_size;
        const int image_height = window_size;

        // Camera
        auto viewport_height = 2.0;
        auto viewport_width = viewport_height;
        auto focal_length = 1.0;

        auto origin = glm::vec3(0, 0, 0);
        auto horizontal = glm::vec3(viewport_width, 0, 0);
        auto vertical = glm::vec3(0, viewport_height, 0);
        auto lower_left_corner = origin - horizontal / glm::vec1(2) - vertical / glm::vec1(2) - glm::vec3(0, 0, focal_length);

        // Render
        for (int j = image_height - 1; j >= 0; --j)
        {
            for (int i = 0; i < image_width; ++i)
            {
                glm::f32 u = double(i) / (image_width - 1);
                glm::f32 v = double(j) / (image_height - 1);
                Ray r(origin, lower_left_corner + u * horizontal + v * vertical - origin);
                glm::vec3 pixel_color = get_color(r);
                writeColor(pixels, glm::vec2(image_width, image_height), glm::vec2(i, j), pixel_color);
            }
        }
    }

    void PathTracer::writeColor(unsigned char *pixels, glm::ivec2 tex_size, glm::ivec2 tex_coord, glm::vec3 color)
    {
        pixels[3 * (tex_size.x * tex_coord.y + tex_coord.x) + 0] = static_cast<int>(color.r * 255.999);
        pixels[3 * (tex_size.x * tex_coord.y + tex_coord.x) + 1] = static_cast<int>(color.g * 255.999);
        pixels[3 * (tex_size.x * tex_coord.y + tex_coord.x) + 2] = static_cast<int>(color.b * 255.999);
    }
}