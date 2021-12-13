#include <GL/glew.h>

#include "camera.hh"
#include "init.hh"
#include "program.hh"
#include <glm/glm.hpp>
#include <iostream>
#include <stdlib.h>

constexpr int MESH_HEIGHT = 5;
constexpr int MESH_WIDTH = 4;

constexpr int PATCH_HEIGHT = 10 * MESH_HEIGHT;
constexpr int PATCH_WIDTH = 10 * MESH_WIDTH;

constexpr double Z_TRANSLATION_SCALE = 1;

struct MeshNode
{
    glm::vec3 pos = {0, 0, 0};
};

std::vector<MeshNode> mesh_nodes;
std::vector<MeshNode> patch_nodes(PATCH_HEIGHT* PATCH_WIDTH);

void update_mesh_nodes()
{
    if (mesh_nodes.empty())
    {
        for (int y = 0; y < MESH_HEIGHT; ++y)
        {
            for (int x = 0; x < MESH_WIDTH; ++x)
                mesh_nodes.push_back({{x, y, 0}});
        }
    }

    //    for (auto& node : mesh_nodes)
    //    {
    //        node.pos.z += Z_TRANSLATION_SCALE * (2 * (double)rand() / INT_MAX - 1);
    //    }
}

GLuint vao_id;
GLuint mesh_node_ssbo_id;
GLuint patch_node_ssbo_id;

void display()
{
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_PROGRAM_POINT_SIZE);

    glm::mat4 world_to_cam_matrix = mygl::Camera::get_instance()->get_world_to_cam_matrix(false);
    glm::mat4 model = glm::mat4(1.0f);
    world_to_cam_matrix = world_to_cam_matrix * model;
    glUniformMatrix4fv(glGetUniformLocation(mygl::Programs::get_instance()->get_id(0), "world_to_cam_matrix"), 1, GL_FALSE, &world_to_cam_matrix[0][0]);

    glPointSize(10);

    // Draw particles

    mygl::Programs().get_instance()->use(1);

    update_mesh_nodes();

    glGenVertexArrays(1, &vao_id);
    glBindVertexArray(vao_id);

    glGenBuffers(1, &mesh_node_ssbo_id);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, mesh_node_ssbo_id);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(struct MeshNode) * mesh_nodes.size(), mesh_nodes.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, mesh_node_ssbo_id);

    glGenBuffers(1, &patch_node_ssbo_id);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, patch_node_ssbo_id);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(struct MeshNode) * patch_nodes.size(), patch_nodes.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, patch_node_ssbo_id);

    glDispatchCompute(PATCH_WIDTH, PATCH_HEIGHT, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    mygl::Programs().get_instance()->use(0);

    glBindBuffer(GL_ARRAY_BUFFER, patch_node_ssbo_id);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), 0);

    glDrawArrays(GL_POINTS, 0, PATCH_HEIGHT * PATCH_WIDTH);
    glutSwapBuffers();

    glutPostRedisplay();
}

int main(int argc, char* argv[])
{
    init_context(argc, argv);

    init_display_program("../scenes/bezier_patch/bezier_patch.vs", "../scenes/bezier_patch/bezier_patch.fs");
    init_compute_program("../scenes/bezier_patch/bezier_patch.cs");

    glutDisplayFunc(display);
    glutMainLoop();

    return 0;
}