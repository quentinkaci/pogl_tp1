#pragma once

#include "../matrix4.hh"
#include "../obj_loader.hh"
#include "../program.hh"
#include "../vec3.hh"
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

static GLuint vao_id;
static GLuint vbo_vertex_ids[3];

// Uniform variables

static float light_position[] = {0.f, 1.f, -0.75f, 1.f};

std::size_t nb_vertices;

inline void initUniformVariables()
{
    GLint loc = glGetUniformLocation(mygl::program::get_instance()->get_id(), "light_position");
    if (loc != -1)
        glUniform4f(loc, light_position[0], light_position[1], light_position[2], light_position[3]);
}

inline void initVAO()
{
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    mygl::ObjLoader::load_obj("../shaders/cube.obj", vertices, uvs, normals);
    nb_vertices = vertices.size();

    glGenVertexArrays(1, &vao_id);
    glBindVertexArray(vao_id);

    glGenBuffers(2, vbo_vertex_ids);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex_ids[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex_ids[1]);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals.data(), GL_STATIC_DRAW);
    // glEnableVertexAttribArray(1);
    // glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, 4 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex_ids[1]);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

inline void display()
{
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 world_to_cam_matrix = mygl::camera::get_instance()->get_world_to_cam_matrix();

    glUniformMatrix4fv(glGetUniformLocation(mygl::program::get_instance()->get_id(), "world_to_cam_matrix"), 1, GL_FALSE, &world_to_cam_matrix[0][0]);

    glDrawArrays(GL_TRIANGLES, 0, nb_vertices);

    glutSwapBuffers();
}