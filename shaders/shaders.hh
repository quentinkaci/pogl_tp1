#pragma once

#include "../matrix4.hh"
#include "../program.hh"
#include <GL/gl.h>

static const GLfloat vertices[] = {-0.5f, -0.5f, 0.0f, 1.0f,
                                   0.5f, -0.5f, 0.0f, 1.0f,
                                   0.0f, 0.5f, 0.0f, 1.0f};

static GLuint vao_id;
static GLuint vbo_vertex_id;

inline void initUniformVariables(const std::shared_ptr<mygl::program>& program)
{
    GLint loc = glGetUniformLocation(program->get_id(), "world_to_cam_matrix");
    if (loc != -1)
        glUniformMatrix4fv(loc, 1, GL_FALSE, mygl::matrix4::identity().data.data());

    loc = glGetUniformLocation(program->get_id(), "uniform_color");
    if (loc != -1)
        glUniform4f(loc, 1, 0, 0, 1);
}

inline void initVAO()
{
    glGenVertexArrays(1, &vao_id);
    glBindVertexArray(vao_id);

    glGenBuffers(1, &vbo_vertex_id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

inline void display()
{
    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glutSwapBuffers();
}