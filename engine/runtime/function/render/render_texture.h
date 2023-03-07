#pragma once

#include <glm/glm.hpp>
#include <stb_image.h>

#include <string>

namespace MiniEngine
{
    class Texture
    {
    public:
        virtual glm::vec3 value(float u, float v, const glm::vec3 &p) const = 0;
    };

    class Image : public Texture
    {
    public:
        const static int bytes_per_pixel = 3;

        Image() : data(nullptr), width(0), height(0), bytes_per_scanline(0) {}

        Image(const char *filename)
        {
            auto components_per_pixel = bytes_per_pixel;

            stbi_set_flip_vertically_on_load(true);

            char buff[1024];
            memset(buff,0,sizeof(buff));
            strcpy(buff,filename);

            data = stbi_load(buff, &width, &height, &components_per_pixel, components_per_pixel);

            if (!data) {
                // std::cerr << "ERROR: Could not load texture image file '" << filename << "'.\n";
                width = height = 0;
            }

            bytes_per_scanline = bytes_per_pixel * width;
        }

        ~Image()
        {
            delete data;
        }

        virtual glm::vec3 value(float u, float v, const glm::vec3 &p) const override
        {
            // If we have no texture data then fallback to mat.Kd
            if (data == nullptr)
                return glm::vec3(-1, -1, -1);

            // Repeat tile mode
            u = u - floor(u);
            v = v - floor(v);

            auto i = static_cast<int>(u * width);
            auto j = static_cast<int>(v * height);

            // Clamp integer mapping, since actual coordinates should be less than 1.0
            if (i >= width)
                i = width - 1;
            if (j >= height)
                j = height - 1;

            const auto color_scale = 1.0 / 255.0;
            auto pixel = data + j * bytes_per_scanline + i * bytes_per_pixel;
            
            // Gama correct
            glm::vec3 linear_color;
            linear_color.r = pow(color_scale * pixel[0], 2.2);
            linear_color.g = pow(color_scale * pixel[1], 2.2);
            linear_color.b = pow(color_scale * pixel[2], 2.2);

            return linear_color;
        }

    private:
        unsigned char *data;
        int width, height;
        int bytes_per_scanline;
    };
} // namespace MiniEngine
