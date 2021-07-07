#pragma once

#include "../matrix4.hh"
#include "../program.hh"
#include "../vec3.hh"
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

const float camera_speed = 0.05f;
//mygl::vec3 camera_pos({0.0f, 0.0f, 0.0f});
//mygl::vec3 camera_front({0.0f, 0.0f, -1.0f});
//mygl::vec3 camera_up({0.0f, 1.0f, 0.0f});
glm::vec3 camera_pos = glm::vec3(0.0f, 0.0f, 1.f);
glm::vec3 camera_front = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camera_up = glm::vec3(0.0f, 1.0f, 0.0f);

static float light_position[] = {0.f, 1.f, -0.75f, 1.f};

std::shared_ptr<mygl::program> program = nullptr;

inline void setProgram(const std::shared_ptr<mygl::program>& p)
{
    program = p;
}

inline void initUniformVariables(const std::shared_ptr<mygl::program>& program)
{
    GLint loc = glGetUniformLocation(program->get_id(), "light_position");
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

void SpecialKeyHandler(int key, int, int)
{
    if (key == GLUT_KEY_UP)
    {
        std::cout << "UP" << std::endl;
        camera_pos += camera_speed * camera_front;
    }
    else if (key == GLUT_KEY_DOWN)
        camera_pos -= camera_speed * camera_front;
    else if (key == GLUT_KEY_LEFT)
        camera_pos -= glm::normalize(glm::cross(camera_front, camera_up)) * camera_speed;
    else if (key == GLUT_KEY_RIGHT)
        camera_pos += glm::normalize(glm::cross(camera_front, camera_up)) * camera_speed;

    glutPostRedisplay();
}

inline void display()
{
    glClearColor(0.0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //    mygl::matrix4 world_to_cam_matrix = mygl::matrix4::identity();
    glm::mat4 view = glm::lookAt(camera_pos, camera_pos + camera_front, camera_up);
    //    look_at(world_to_cam_matrix, camera_pos, camera_pos + camera_front, camera_up);
    glUniformMatrix4fv(glGetUniformLocation(program->get_id(), "world_to_cam_matrix"), 1, GL_FALSE, &view[0][0]);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glutSwapBuffers();
}