#pragma once

#include <fstream>
#include <glm/glm.hpp>
#include <iostream>
#include <string.h>
#include <string>
#include <vector>

namespace mygl
{
    class ObjLoader
    {
    public:
        static void load_obj(const std::string& path,
                             std::vector<glm::vec3>& vertices,
                             std::vector<glm::vec2>& uvs,
                             std::vector<glm::vec3>& normals)
        {
            std::vector<size_t> vertex_pos, uv_pos, normal_pos;
            std::vector<glm::vec3> base_vertices, base_normals;
            std::vector<glm::vec2> base_uvs;

            std::ifstream file(path);
            if (!file.is_open())
            {
                std::cerr << "OBJ file " << path << " doesn't exist" << std::endl;
                exit(1);
            }

            while (!file.eof())
            {
                std::string line;
                std::getline(file, line);

                std::string code = line.substr(0, line.find(" "));
                if (code == "v")
                {
                    glm::vec3 vertex;
                    std::sscanf(line.c_str(), "v %f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
                    base_vertices.push_back(vertex);
                }
                else if (code == "vt")
                {
                    glm::vec2 uv;
                    std::sscanf(line.c_str(), "vt %f %f\n", &uv.x, &uv.y);
                    uv.y = 1.0f - uv.y;
                    base_uvs.push_back(uv);
                }
                else if (code == "vn")
                {
                    glm::vec3 normal;
                    std::sscanf(line.c_str(), "vn %f %f %f\n", &normal.x, &normal.y, &normal.z);
                    base_normals.push_back(normal);
                }
                else if (code == "f")
                {
                    std::string vertex1, vertex2, vertex3;
                    unsigned int vertex_indices[3], uv_indices[3], normal_indices[3];
                    std::sscanf(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d\n", &vertex_indices[0], &uv_indices[0], &normal_indices[0], &vertex_indices[1], &uv_indices[1], &normal_indices[1], &vertex_indices[2], &uv_indices[2], &normal_indices[2]);

                    for (int i = 0; i < 3; ++i)
                    {
                        vertex_pos.push_back(vertex_indices[i]);
                        uv_pos.push_back(uv_indices[i]);
                        normal_pos.push_back(normal_indices[i]);
                    }
                }
            }

            for (unsigned int i = 0; i < vertex_pos.size(); i++)
            {
                unsigned int vertex_indices = vertex_pos[i];
                unsigned int uv_indices = uv_pos[i];
                unsigned int normal_indices = normal_pos[i];

                vertices.push_back(base_vertices[vertex_indices - 1]);
                uvs.push_back(base_uvs[uv_indices - 1]);
                normals.push_back(base_normals[normal_indices - 1]);
            }
        }
    };
} // namespace mygl