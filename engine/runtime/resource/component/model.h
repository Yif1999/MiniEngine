#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <tiny_obj_loader.h>

#include <runtime/resource/component/mesh.h>
#include <runtime/resource/component/shader.h>
#include <runtime/core/base/hash.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <unordered_map>

namespace MiniEngine
{
    unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);

    class Model 
    {
    public:
        // model data 
        vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
        vector<Mesh>    meshes;
        string directory;
        bool gammaCorrection;

        // constructor, expects a filepath to a 3D model.
        Model(string const &path, bool gamma = false) : gammaCorrection(gamma)
        {
            loadModel(path);
        }

        // draws the model, and thus all its meshes
        void Draw(Shader &shader)
        {
            for(unsigned int i = 0; i < meshes.size(); i++)
                meshes[i].Draw(shader);
        }
        
    private:
        // loads a model with supported tiny_obj_loader extensions from file and stores the resulting meshes in the meshes vector.
        void loadModel(string const &path)
        {
            // read file
            directory = path.substr(0, path.find_last_of('/'));
            std::string inputfile = path;
            tinyobj::ObjReaderConfig reader_config;
            reader_config.mtl_search_path = directory; 

            tinyobj::ObjReader reader;

            // check for errors
            if (!reader.ParseFromFile(inputfile, reader_config)) {
            if (!reader.Error().empty()) {
                std::cerr << "TinyObjReader: " << reader.Error();
            }
            exit(1);
            }

            if (!reader.Warning().empty()) {
            std::cout << "TinyObjReader: " << reader.Warning();
            }

            // traverse the meshes
            loadMesh(reader);
        }

        void loadMesh(tinyobj::ObjReader& reader)
        {
            // extract data of reader
            auto& attrib = reader.GetAttrib();
            auto& shapes = reader.GetShapes();
            auto& materials = reader.GetMaterials();

#ifdef DEBUG_OBJLOADER
            std::cout << "顶点数：" << attrib.vertices.size() / 3 << std::endl;
            std::cout << "法线数：" << attrib.normals.size() / 3 << std::endl;
            std::cout << "UV数：" << attrib.texcoords.size() / 2 << std::endl;
            std::cout << "子模型数：" << shapes.size() << std::endl;
            std::cout << "材质数：" << materials.size() << std::endl;
#endif
            
            // loop the meshes
            for (size_t s = 0; s < shapes.size(); s++)
            {
                // data to fill
                vector<Vertex> vertices;
                vector<unsigned int> indices;
                vector<Texture> textures;

                std::unordered_map<Vertex, unsigned int> uniqueVertices = {};
                size_t index_offset = 0;
                for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
                {
                    size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

                    bool with_normal   = true;
                    bool with_texcoord = true;

                    glm::vec3 vertex[3];
                    glm::vec3 normal[3];
                    glm::vec2 uv[3];

                    // only deals with triangle faces
                    if (fv != 3)
                    {
                        continue;
                    }

                    // expanding vertex data is not efficient and will deduplicate later
                    for (size_t v = 0; v < fv; v++)
                    {
                        auto idx = shapes[s].mesh.indices[index_offset + v];
                        auto vx  = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
                        auto vy  = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
                        auto vz  = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

                        vertex[v].x = static_cast<float>(vx);
                        vertex[v].y = static_cast<float>(vy);
                        vertex[v].z = static_cast<float>(vz);

                        if (idx.normal_index >= 0)
                        {
                            auto nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
                            auto ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
                            auto nz = attrib.normals[3 * size_t(idx.normal_index) + 2];

                            normal[v].x = static_cast<float>(nx);
                            normal[v].y = static_cast<float>(ny);
                            normal[v].z = static_cast<float>(nz);
                        }
                        else
                        {
                            with_normal = false;
                        }

                        if (idx.texcoord_index >= 0)
                        {
                            auto tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
                            auto ty = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];

                            uv[v].x = static_cast<float>(tx);
                            uv[v].y = static_cast<float>(ty);
                        }
                        else
                        {
                            with_texcoord = false;
                        }
                    }
                    index_offset += fv;

                    if (!with_normal)
                    {
                        glm::vec3 v0 = vertex[1] - vertex[0];
                        glm::vec3 v1 = vertex[2] - vertex[1];
                        normal[0]  = glm::normalize(v0*v1);
                        normal[1]  = normal[0];
                        normal[2]  = normal[0];
                    }

                    if (!with_texcoord)
                    {
                        uv[0] = glm::vec2(0.5f, 0.5f);
                        uv[1] = glm::vec2(0.5f, 0.5f);
                        uv[2] = glm::vec2(0.5f, 0.5f);
                    }

                    glm::vec3 tangent {1, 0, 0};
                    {
                        glm::vec3 edge1    = vertex[1] - vertex[0];
                        glm::vec3 edge2    = vertex[2] - vertex[1];
                        glm::vec2 deltaUV1 = uv[1] - uv[0];
                        glm::vec2 deltaUV2 = uv[2] - uv[1];

                        auto divide = deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y;
                        if (divide >= 0.0f && divide < 0.000001f)
                            divide = 0.000001f;
                        else if (divide < 0.0f && divide > -0.000001f)
                            divide = -0.000001f;

                        float df  = 1.0f / divide;
                        tangent.x = df * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
                        tangent.y = df * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
                        tangent.z = df * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
                        glm::normalize(tangent);
                    }

                    for (size_t i = 0; i < 3; i++)
                    {
                        Vertex mesh_vert;

                        mesh_vert.Position.x = vertex[i].x;
                        mesh_vert.Position.y = vertex[i].y;
                        mesh_vert.Position.z = vertex[i].z;

                        mesh_vert.Normal.x = normal[i].x;
                        mesh_vert.Normal.y = normal[i].y;
                        mesh_vert.Normal.z = normal[i].z;

                        mesh_vert.TexCoords.x = uv[i].x;
                        mesh_vert.TexCoords.y = uv[i].y;

                        mesh_vert.Tangent.x = tangent.x;
                        mesh_vert.Tangent.y = tangent.y;
                        mesh_vert.Tangent.z = tangent.z;

                        if (uniqueVertices.count(mesh_vert) == 0) {
                            uniqueVertices[mesh_vert] = static_cast<unsigned int>(vertices.size());
                            vertices.push_back(mesh_vert);
                        }

                        indices.push_back(uniqueVertices[mesh_vert]);
                    }
                }

                meshes.push_back(Mesh(vertices, indices, textures));
            }
        }
    };
} // MiniEngine namespace
#endif