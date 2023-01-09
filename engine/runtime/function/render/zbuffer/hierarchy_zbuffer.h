#pragma once

#include "runtime/function/render/render_model.h"
#include "runtime/function/render/zbuffer/transform.h"

#define window_size 512

namespace MiniEngine
{
    struct Mipmap
    {
    public:
        int flag;
        float depth;
        Mipmap *parent;
        Mipmap *childs[4];

        Mipmap()
        {
            flag = 0;
            depth = 0;

            parent = nullptr;
            childs[0] = nullptr;
            childs[1] = nullptr;
            childs[2] = nullptr;
            childs[3] = nullptr;
        };

        Mipmap(Mipmap *c0, Mipmap *c1, Mipmap *c2, Mipmap *c3)
        {
            flag = 0;
            depth = 0;

            c0->parent = this;
            c1->parent = this;
            c2->parent = this;
            c3->parent = this;
            childs[0] = c0;
            childs[1] = c1;
            childs[2] = c2;
            childs[3] = c3;
        };
    };

    struct EdgeEquation
    {
        float a;
        float b;
        float c;
        bool tie;

        EdgeEquation(const Vertex &v0, const Vertex &v1)
        {
            a = v0.Position.y - v1.Position.y;
            b = v1.Position.x - v0.Position.x;
            c = -(a * (v0.Position.x + v1.Position.x) + b * (v0.Position.y + v1.Position.y)) / 2;
            tie = a != 0 ? a > 0 : b > 0;
        }

        // Test if the given point is inside the edge.
        bool evaluate(float x, float y)
        {
            float t = a * x + b * y + c;
            return (t > 0 || t == 0 && tie);
        }
    };

    struct ParameterEquation
    {
        float a;
        float b;
        float c;

        ParameterEquation(
            float p0,
            float p1,
            float p2,
            const EdgeEquation &e0,
            const EdgeEquation &e1,
            const EdgeEquation &e2,
            float area)
        {
            float factor = 1.0f / (2.0f * area);

            a = factor * (p0 * e0.a + p1 * e1.a + p2 * e2.a);
            b = factor * (p0 * e0.b + p1 * e1.b + p2 * e2.b);
            c = factor * (p0 * e0.c + p1 * e1.c + p2 * e2.c);
        }

        /// compute the parameter equation for the given point.
        float interpolate(float x, float y)
        {
            return a * x + b * y + c;
        }
    };

    void hierarchy_zbuffer_rasterize(
        Model *m_model,
        glm::mat4 &model,
        glm::mat4 &view,
        glm::mat4 &projection,
        unsigned char *pixels,
        unsigned char *texture,
        int width,
        int height);

    void hierarchy_zbuffer_initialize(int size);

    Mipmap* build_mipmap(int size);

    void triangle_render(Model *m_model,
                         unsigned char *pixels,
                         unsigned char *texture,
                         int width, 
                         int height);

    bool ztest(float z, int x, int y);
}