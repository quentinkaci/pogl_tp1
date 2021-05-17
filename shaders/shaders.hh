#pragma once

#include "../matrix4.hh"
#include "../program.hh"
#include <GL/gl.h>

inline void initUniformVariables(const std::shared_ptr<mygl::program>& program)
{
    GLint loc = glGetUniformLocation(program->get_id(), "world_to_cam_matrix");
    if (loc != -1)
        glUniformMatrix4fv(loc, 16, GL_FALSE, mygl::matrix4::identity().data.data());

    loc = glGetUniformLocation(program->get_id(), "uniform_color");
    if (loc != -1)
        glUniform4f(loc, 255, 0, 0, 255);
}

inline void initVAO(const std::vector<float>& vertices)
{
    GLuint vao_id;
    glGenVertexArrays(1, &vao_id);
    glBindVertexArray(vao_id);

    GLuint vbo_vertex_id;
    glGenBuffers(1, &vbo_vertex_id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex_id);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(vbo_vertex_id, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
}

inline void display()
{
    glFlush();
}