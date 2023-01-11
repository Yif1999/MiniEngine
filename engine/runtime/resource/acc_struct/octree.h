#pragma once

#include "runtime/function/render/render_model.h"

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include <memory>
#include <vector>
#include <iostream>

#define MaxFaceNum 1000

namespace MiniEngine
{
    class OctTree
    {
    public:
        OctTree() = default;

        struct Bound
        {
            glm::vec3 corner[8];
        };

        struct OctNode
        {
            Mesh triangles;
            Bound bound[2]; // two sets bbox: 1 for splitting, 2 for actual info.
            bool visibility = false;
            bool empty = false;

            OctNode *parent;
            OctNode *childs[8];

            OctNode()=default;
            OctNode(Mesh triangles)
            {
                this->triangles = triangles;
                this->parent = nullptr;
                this->childs[0] = nullptr;
                this->childs[1] = nullptr;
                this->childs[2] = nullptr;
                this->childs[3] = nullptr;
                this->childs[4] = nullptr;
                this->childs[5] = nullptr;
                this->childs[6] = nullptr;
                this->childs[7] = nullptr;
            }
        };

        OctNode build_oct_tree(std::shared_ptr<Model> m_model)
        {
            Mesh mesh = m_model->meshes[0];

            OctNode model_root = OctNode(mesh);

            float xmin, xmax, ymin, ymax, zmin, zmax;
            xmin = mesh.vertices[0].Position.x-0.1;
            xmax = mesh.vertices[0].Position.x+0.1;
            ymin = mesh.vertices[0].Position.y-0.1;
            ymax = mesh.vertices[0].Position.y+0.1;
            zmin = mesh.vertices[0].Position.z-0.1;
            zmax = mesh.vertices[0].Position.z+0.1;

            for (int i = 1; i < mesh.vertices.size(); i++)
            {
                if (mesh.vertices[i].Position.x < xmin)
                    xmin = mesh.vertices[i].Position.x;
                if (mesh.vertices[i].Position.x > xmax)
                    xmax = mesh.vertices[i].Position.x;
                if (mesh.vertices[i].Position.y < ymin)
                    ymin = mesh.vertices[i].Position.y;
                if (mesh.vertices[i].Position.y > ymax)
                    ymax = mesh.vertices[i].Position.y;
                if (mesh.vertices[i].Position.z < zmin)
                    zmin = mesh.vertices[i].Position.z;
                if (mesh.vertices[i].Position.z > zmax)
                    zmax = mesh.vertices[i].Position.z;
            }

            Bound bound;

            bound.corner[0] = glm::vec3(xmax, ymax, zmax);
            bound.corner[1] = glm::vec3(xmax, ymax, zmin);
            bound.corner[2] = glm::vec3(xmax, ymin, zmax);
            bound.corner[3] = glm::vec3(xmin, ymax, zmax);
            bound.corner[4] = glm::vec3(xmin, ymin, zmax);
            bound.corner[5] = glm::vec3(xmin, ymax, zmin);
            bound.corner[6] = glm::vec3(xmax, ymin, zmin);
            bound.corner[7] = glm::vec3(xmin, ymin, zmin);

            space_split(&model_root, nullptr, bound, -1);

            return model_root;
        }

    private:
        static int get_triangle_num(Mesh mesh)
        {
            return (mesh.indices.size() / 3);
        }

        static void space_split(OctNode *node, OctNode *parent, Bound bound, int id)
        {
            node->bound[0] = bound;

            if (id < 0)
            {
                node->bound[1].corner[0] = node->bound[0].corner[0];
                node->bound[1].corner[1] = node->bound[0].corner[1];
                node->bound[1].corner[2] = node->bound[0].corner[2];
                node->bound[1].corner[3] = node->bound[0].corner[3];
                node->bound[1].corner[4] = node->bound[0].corner[4];
                node->bound[1].corner[5] = node->bound[0].corner[5];
                node->bound[1].corner[6] = node->bound[0].corner[6];
                node->bound[1].corner[7] = node->bound[0].corner[7];
                
            }
            else
            {
                node->parent = parent;
                parent->childs[id] = node;

                if (node->triangles.vertices.size()==0)
                {
                    node->empty=true;
                    return;
                }

                float xmin, xmax, ymin, ymax, zmin, zmax;
                xmin = node->triangles.vertices[0].Position.x;
                xmax = node->triangles.vertices[0].Position.x;
                ymin = node->triangles.vertices[0].Position.y;
                ymax = node->triangles.vertices[0].Position.y;
                zmin = node->triangles.vertices[0].Position.z;
                zmax = node->triangles.vertices[0].Position.z;

                for (int i = 1; i < node->triangles.vertices.size(); i++)
                {
                    if (node->triangles.vertices[i].Position.x < xmin)
                        xmin = node->triangles.vertices[i].Position.x;
                    if (node->triangles.vertices[i].Position.x > xmax)
                        xmax = node->triangles.vertices[i].Position.x;
                    if (node->triangles.vertices[i].Position.y < ymin)
                        ymin = node->triangles.vertices[i].Position.y;
                    if (node->triangles.vertices[i].Position.y > ymax)
                        ymax = node->triangles.vertices[i].Position.y;
                    if (node->triangles.vertices[i].Position.z < zmin)
                        zmin = node->triangles.vertices[i].Position.z;
                    if (node->triangles.vertices[i].Position.z > zmax)
                        zmax = node->triangles.vertices[i].Position.z;
                }

                node->bound[1].corner[0] = glm::vec3(xmax, ymax, zmax);
                node->bound[1].corner[1] = glm::vec3(xmax, ymax, zmin);
                node->bound[1].corner[2] = glm::vec3(xmax, ymin, zmax);
                node->bound[1].corner[3] = glm::vec3(xmin, ymax, zmax);
                node->bound[1].corner[4] = glm::vec3(xmin, ymin, zmax);
                node->bound[1].corner[5] = glm::vec3(xmin, ymax, zmin);
                node->bound[1].corner[6] = glm::vec3(xmax, ymin, zmin);
                node->bound[1].corner[7] = glm::vec3(xmin, ymin, zmin);
            }

            int face_num = get_triangle_num(node->triangles);

            if (face_num > MaxFaceNum)
            {
                Mesh submesh;
                Bound split_bound;
                std::unordered_map<Vertex, unsigned int> uniqueVertices;

                for (int i = 0; i < 8; i++)
                {
                    // clean cache
                    submesh.vertices.clear();
                    submesh.indices.clear();
                    uniqueVertices.clear();

                    // cauculate split bound
                    for (int j = 0; j < 8; j++)
                    {
                        split_bound.corner[j] = (node->bound[0].corner[i] + node->bound[0].corner[j]) / glm::vec1(2);
                    }

                    // sift triangles
                    for (int j = 0; j < face_num; j++)
                    {
                        glm::vec3 point = node->triangles.vertices[node->triangles.indices[3 * j]].Position;
                        if (is_in_bound(point, split_bound))
                        {
                            for (int k = 0; k < 3; k++)
                            {
                                Vertex vertice = node->triangles.vertices[node->triangles.indices[3 * j + k]];
                                if (uniqueVertices.count(vertice) == 0)
                                {
                                    uniqueVertices[vertice] = static_cast<unsigned int>(submesh.vertices.size());
                                    submesh.vertices.push_back(vertice);
                                }
                                submesh.indices.push_back(uniqueVertices[vertice]);
                            }
                        }
                    }

                    // create child node in PM
                    OctNode sub_node = OctNode(submesh);
                    OctNode *child_node =(OctNode*)malloc(sizeof(sub_node));
                    *child_node = sub_node;
                    space_split(child_node, node, split_bound, i);
                }
            }
        }

        static bool is_in_bound(glm::vec3 p, Bound bound)
        {
            float xmin, xmax, ymin, ymax, zmin, zmax;

            xmin = min({bound.corner[0].x, bound.corner[1].x, bound.corner[2].x, bound.corner[3].x, bound.corner[4].x, bound.corner[5].x, bound.corner[6].x, bound.corner[7].x});
            xmax = max({bound.corner[0].x, bound.corner[1].x, bound.corner[2].x, bound.corner[3].x, bound.corner[4].x, bound.corner[5].x, bound.corner[6].x, bound.corner[7].x});
            ymin = min({bound.corner[0].y, bound.corner[1].y, bound.corner[2].y, bound.corner[3].y, bound.corner[4].y, bound.corner[5].y, bound.corner[6].y, bound.corner[7].y});
            ymax = max({bound.corner[0].y, bound.corner[1].y, bound.corner[2].y, bound.corner[3].y, bound.corner[4].y, bound.corner[5].y, bound.corner[6].y, bound.corner[7].y});
            zmin = min({bound.corner[0].z, bound.corner[1].z, bound.corner[2].z, bound.corner[3].z, bound.corner[4].z, bound.corner[5].z, bound.corner[6].z, bound.corner[7].z});
            zmax = max({bound.corner[0].z, bound.corner[1].z, bound.corner[2].z, bound.corner[3].z, bound.corner[4].z, bound.corner[5].z, bound.corner[6].z, bound.corner[7].z});

            if (xmin < p.x && p.x <= xmax &&
                ymin < p.y && p.y <= ymax &&
                zmin < p.z && p.z <= zmax)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    };
}