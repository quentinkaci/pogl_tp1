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

// VBO

static const float vertices_[] = {-0.5f, -0.5f, 0.f, 1.f,
                                  0.5f, -0.5f, 0.f, 1.f,
                                  0.f, 0.5f, 0.f, 1.f};
static const float normals_[] = {0.f, 0.f, -1.f, 1.f,
                                 0.f, 0.f, -1.f, 1.f,
                                 0.f, 0.f, -1.f, 1.f};
static const float colors[] = {1.f, 0.f, 0.f, 1.f,
                               0.f, 1.f, 0.f, 1.f,
                               0.f, 0.f, 1.f, 1.f};

// Camera settings

const float camera_speed = 0.05f;

glm::vec3 camera_pos = glm::vec3({0.0f, 0.0f, 1.f});
glm::vec3 camera_front = glm::vec3({0.0f, 0.0f, -1.0f});
glm::vec3 camera_up = glm::vec3({0.0f, 1.0f, 0.0f});

bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 512.0f;
float lastY = 512.0f;
float fov = 90.0f;

// Uniform variables

static float light_position[] = {0.f, 1.f, -0.75f, 1.f};

std::shared_ptr<mygl::program> program = nullptr;

std::size_t nb_vertices;

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

    glm::vec3 direction;
    direction[0] = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction[1] = sin(glm::radians(pitch));
    direction[2] = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    camera_front = normalize(direction);

    glutPostRedisplay();
}

void mouse_callback(int button, int state, int x, int y)
{
    if (button == 3)
        fov -= (float)1;
    else if (button == 4)
        fov += (float)1;
    else if (button == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            lastX = x;
            lastY = y;
            firstMouse = false;
        }
        else if (state == GLUT_UP)
            firstMouse = true;
    }

    fov = std::clamp(fov, 1.0f, 90.0f);

    glutPostRedisplay();
}

inline void display()
{
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 projection = glm::perspective(glm::radians(fov), 1.0f, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(camera_pos, camera_pos + camera_front, camera_up);
    glm::mat4 world_to_cam_matrix = projection * view;

    glUniformMatrix4fv(glGetUniformLocation(program->get_id(), "world_to_cam_matrix"), 1, GL_FALSE, &world_to_cam_matrix[0][0]);

    glDrawArrays(GL_TRIANGLES, 0, nb_vertices);

    glutSwapBuffers();
}