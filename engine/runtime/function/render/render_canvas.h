#pragma once

#include <glad/glad.h>

#include "runtime/function/render/render_shader.h"

namespace MiniEngine
{
    class Canvas
    {
    public:
        Canvas(float hw, float hh)
        {
            half_width = hw;
            half_height = hh;

            setupCanvas();
        }

        void Draw(std::shared_ptr<Shader> shader)
        {
            // get path tracer info
            std::shared_ptr<PathTracing::PathTracer> m_path_tracer = g_runtime_global_context.m_render_system->getPathTracer();
            // draw canvas plane
            glBindTexture(GL_TEXTURE_2D, m_path_tracer->result);
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_path_tracer->width, m_path_tracer->height, GL_RGB, GL_UNSIGNED_BYTE, m_path_tracer->pixels);
            glActiveTexture(GL_TEXTURE0);
        }

    private:
        unsigned int VBO, VAO, EBO;

        float half_width;
        float half_height;

        void setupCanvas()
        {
            float vertices[] = {
                half_width, 0.0f,  half_height, 1.f, 0.f,   // top right
                half_width, -0.0f, -half_height, 1.f, 1.f,  // bottom right
                -half_width, -0.0f, -half_height, 0.f, 1.f, // bottom left
                -half_width, 0.0f,  half_height, 0.f, 0.f   // top left
            };
            unsigned int indices[] = {
                0, 1, 3, // first Triangle
                1, 2, 3  // second Triangle
            };

            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);

            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }
    };
}