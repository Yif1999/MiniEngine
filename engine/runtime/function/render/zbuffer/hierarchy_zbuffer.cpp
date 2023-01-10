#include "runtime/function/render/zbuffer/hierarchy_zbuffer.h"

#include <iostream>
#include <cmath>

// #define DEPTH

using namespace std;

namespace MiniEngine
{
    void SoftRasterizer::hierarchy_zbuffer_rasterize(
        Model *m_model,
        glm::mat4 &model,
        glm::mat4 &view,
        glm::mat4 &projection,
        unsigned char *pixels,
        unsigned char *texture,
        int width,
        int height)
    {
        screen_space_transform(m_model, model, view, projection);
        SoftRasterizer::triangle_render(m_model, pixels, texture, width, height);
    }

    void SoftRasterizer::hierarchy_zbuffer_initialize(int size)
    {
        zbuffer = SoftRasterizer::build_mipmap(size);
    }

    SoftRasterizer::Mipmap* SoftRasterizer::build_mipmap(int size)
    {
        vector<vector<vector<Mipmap *>>> texel;
        texel.resize(log2(size) + 1);
        bool is_leafnode = true;

        while (size)
        {
            int level = log2(size);
            texel[level].resize(size);

            if (is_leafnode)
            {
                for (int i = 0; i < size; i++)
                {
                    texel[level][i].resize(size);
                    for (int j = 0; j < size; j++)
                    {
                        texel[level][i][j] = new Mipmap();
                    }
                }
                is_leafnode = false;
            }
            else
            {
                for (int i = 0; i < size; i++)
                {
                    texel[level][i].resize(size);
                    for (int j = 0; j < size; j++)
                    {
                        texel[level][i][j] = new Mipmap(texel[level + 1][2 * i][2 * j],
                                                        texel[level + 1][2 * i][2 * j + 1],
                                                        texel[level + 1][2 * i + 1][2 * j],
                                                        texel[level + 1][2 * i + 1][2 * j + 1]);
                    }
                }
            }

            size = size >> 1;
        }

        return texel[0][0][0];
    }

    void SoftRasterizer::triangle_render(Model *m_model, unsigned char *pixels, unsigned char *texture, int width, int height)
    {
        // refresh zbuffer
        zbuffer->refresh();

        // save vertices of one triangle
        vector<Vertex> vertices(3);

        // loop triangle faces
        for (int id = 0; id < m_model->meshes[0].indices.size(); id += 3)
        {
            vertices[0] = m_model->meshes[0].vertices[m_model->meshes[0].indices[id]];
            vertices[1] = m_model->meshes[0].vertices[m_model->meshes[0].indices[id + 1]];
            vertices[2] = m_model->meshes[0].vertices[m_model->meshes[0].indices[id + 2]];

            vertices[0].Position.x=int(vertices[0].Position.x+0.5);
            vertices[1].Position.x=int(vertices[1].Position.x+0.5);
            vertices[2].Position.x=int(vertices[2].Position.x+0.5);
            vertices[0].Position.y=int(vertices[0].Position.y+0.5);
            vertices[1].Position.y=int(vertices[1].Position.y+0.5);
            vertices[2].Position.y=int(vertices[2].Position.y+0.5);
            
            float az,bz,cz;
            az=vertices[0].Position.z;
            bz=vertices[1].Position.z;
            cz=vertices[2].Position.z;

            float zmin=min({az,bz,cz});

            int xmin, xmax, ymin, ymax;
            xmin = max(int(min({vertices[0].Position.x, vertices[1].Position.x, vertices[2].Position.x})), 0);
            xmax = min(int(max({vertices[0].Position.x, vertices[1].Position.x, vertices[2].Position.x})), window_size -1);
            ymin = max(int(min({vertices[0].Position.y, vertices[1].Position.y, vertices[2].Position.y})), 0);
            ymax = min(int(max({vertices[0].Position.y, vertices[1].Position.y, vertices[2].Position.y})), window_size -1);

            EdgeEquation e0(vertices[1], vertices[2]);
            EdgeEquation e1(vertices[2], vertices[0]);
            EdgeEquation e2(vertices[0], vertices[1]);

            // skip backface
            float area = 0.5 * (e0.c + e1.c + e2.c);
            if (area < 0.1)
                continue;

            ParameterEquation depth(vertices[0].Position.z,
                                    vertices[1].Position.z,
                                    vertices[2].Position.z,
                                    e0, e1, e2, area);
            ParameterEquation texcoord_s(vertices[0].TexCoords.s,
                                         vertices[1].TexCoords.s,
                                         vertices[2].TexCoords.s,
                                         e0, e1, e2, area);
            ParameterEquation texcoord_t(vertices[0].TexCoords.t,
                                         vertices[1].TexCoords.t,
                                         vertices[2].TexCoords.t,
                                         e0, e1, e2, area);

            for (int px = xmin; px <= xmax; px++)
            {
                for (int py = ymin; py <= ymax; py++)
                {
                    if (e0.evaluate(px, py) && e1.evaluate(px, py) && e2.evaluate(px, py))
                    {
                        float z = depth.interpolate(px, py);

                        if (ztest(z,px,py))  
                        {
                            float u = texcoord_s.interpolate(px, py);
                            float v = texcoord_t.interpolate(px, py);
#ifdef DEPTH
                            pixels[3 * (window_size * py + px) + 0] = z*16+32;
                            pixels[3 * (window_size * py + px) + 1] = z*16+32;
                            pixels[3 * (window_size * py + px) + 2] = z*16+32;
#else             
                            pixels[3 * (window_size * py + px) + 0] = texture[3 * (width * int(height * v + 0.5) + int(width * u + 0.5)) + 0];
                            pixels[3 * (window_size * py + px) + 1] = texture[3 * (width * int(height * v + 0.5) + int(width * u + 0.5)) + 1];
                            pixels[3 * (window_size * py + px) + 2] = texture[3 * (width * int(height * v + 0.5) + int(width * u + 0.5)) + 2];
#endif
                        }
                    }
                }
            }
        }
    }

    bool SoftRasterizer::ztest(float z, int x, int y)
    {
        int texel_size= window_size;
        auto mipmap = zbuffer;

        while (mipmap->childs[0] != nullptr)
        {
            texel_size=texel_size>>1;

            if (mipmap->depth <= z)
            {
                return false;
            }
            else
            {
                if (x<texel_size && y<texel_size)
                {
                    mipmap=mipmap->childs[0];
                }
                else if (x>=texel_size && y<texel_size)
                {
                    x-=texel_size;
                    mipmap=mipmap->childs[1];
                }
                else if (x<texel_size && y>=texel_size)
                {
                    y-=texel_size;
                    mipmap=mipmap->childs[2];
                }
                else if (x>=texel_size && y>=texel_size)
                {
                    x-=texel_size;
                    y-=texel_size;
                    mipmap=mipmap->childs[3];
                }
            }
        }

        mipmap->update(z);
        
        return true;    

    }
}