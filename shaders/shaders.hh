#pragma once

#include "../matrix4.hh"
#include "../program.hh"
#include <GL/gl.h>

static GLuint vao_id;
static GLuint vbo_vertex_ids[3];

// VBO

static const float vertices[] = {-0.5f, -0.5f, 0.f, 1.f,
                                 0.5f, -0.5f, 0.f, 1.f,
                                 0.f, 0.5f, 0.f, 1.f};
static const float normals[] = {0.f, 0.f, -1.f, 1.f,
                                0.f, 0.f, -1.f, 1.f,
                                0.f, 0.f, -1.f, 1.f};
static const float colors[] = {1.f, 0.f, 0.f, 1.f,
                               0.f, 1.f, 0.f, 1.f,
                               0.f, 0.f, 1.f, 1.f};

// Uniform variables

static mygl::matrix4 world_to_cam_matrix = mygl::matrix4::identity();
static float light_position[] = {0.f, 1.f, -0.75f, 1.f};

inline void initUniformVariables(const std::shared_ptr<mygl::program>& program)
{
    GLint loc = glGetUniformLocation(program->get_id(), "world_to_cam_matrix");
    if (loc != -1)
        glUniformMatrix4fv(loc, 1, GL_FALSE, world_to_cam_matrix.data.data());

    loc = glGetUniformLocation(program->get_id(), "light_position");
    if (loc != -1)
        glUniform4f(loc, light_position[0], light_position[1], light_position[2], light_position[3]);
}

inline void initVAO()
{
    glGenVertexArrays(1, &vao_id);
    glBindVertexArray(vao_id);

    glGenBuffers(3, vbo_vertex_ids);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex_ids[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex_ids[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, 4 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex_ids[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
}

inline void display()
{
    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glutSwapBuffers();
}