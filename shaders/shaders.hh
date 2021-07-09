#pragma once

#include "../image.hh"
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

// Camera settings

const float camera_speed = 0.05f;

mygl::vec3 camera_pos = mygl::vec3({0.0f, 0.0f, 1.f});
mygl::vec3 camera_front = mygl::vec3({0.0f, 0.0f, -1.0f});
mygl::vec3 camera_up = mygl::vec3({0.0f, 1.0f, 0.0f});

bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 512.0f;
float lastY = 512.0f;
float fov = 90.0f;

// Uniform variables

static float light_position[] = {0.f, 1.f, -0.75f, 1.f};

std::shared_ptr<mygl::program> program = nullptr;

inline void setProgram(const std::shared_ptr<mygl::program>& p)
{
    program = p;
}

inline void initUniformVariables()
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

void initTextures(const std::vector<std::string>& textures)
{
    for (std::size_t i = 0; i < textures.size(); ++i)
    {
        std::string file_delimiter = ".";
        std::string texture_name = textures[i].substr(0, textures[i].find(file_delimiter));

        auto texture = PNGImage::load(textures[i]);
        GLint texture_loc;
        GLuint texture_id;

        glGenTextures(1, &texture_id);
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture.width, texture.height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture.pixels.data());
        texture_loc = glGetUniformLocation(program->get_id(), (texture_name + "_sampler").c_str());
        glUniform1i(texture_loc, i);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
}

void key_callback(int key, int, int)
{
    if (key == GLUT_KEY_UP)
        camera_pos = camera_pos + camera_speed * camera_front;
    else if (key == GLUT_KEY_DOWN)
        camera_pos = camera_pos - camera_speed * camera_front;
    else if (key == GLUT_KEY_LEFT)
        camera_pos = camera_pos - normalize(cross(camera_front, camera_up)) * camera_speed;
    else if (key == GLUT_KEY_RIGHT)
        camera_pos = camera_pos + normalize(cross(camera_front, camera_up)) * camera_speed;

    glutPostRedisplay();
}

void mouse_motion_callback(int x, int y)
{
    if (firstMouse)
    {
        lastX = x;
        lastY = y;
        firstMouse = false;
    }

    float xoffset = x - lastX;
    float yoffset = lastY - y;
    lastX = x;
    lastY = y;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    pitch = std::clamp(pitch, -89.0f, 89.0f);

    mygl::vec3 direction;
    direction[0] = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction[1] = sin(glm::radians(pitch));
    direction[2] = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    camera_front = normalize(direction);

    glutPostRedisplay();
}

void mouse_scroll_callback(int button, int, int, int)
{
    if (button == 3)
        fov -= (float)1;
    else if (button == 4)
        fov += (float)1;

    fov = std::clamp(fov, 1.0f, 90.0f);

    glutPostRedisplay();
}

inline void display()
{
    glClearColor(0.0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mygl::matrix4 world_to_cam_matrix = mygl::matrix4::identity();
    perspective(world_to_cam_matrix, fov, 1.0f, 0.1f, 100.0f);
    look_at(world_to_cam_matrix, camera_pos, camera_pos + camera_front, camera_up);

    glUniformMatrix4fv(glGetUniformLocation(program->get_id(), "world_to_cam_matrix"), 1, GL_FALSE, world_to_cam_matrix.data.data());

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glutSwapBuffers();
}