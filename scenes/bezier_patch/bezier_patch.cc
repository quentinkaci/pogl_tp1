#include <GL/glew.h>

#include "camera.hh"
#include "init.hh"
#include "program.hh"
#include <glm/glm.hpp>
#include <stdlib.h>

constexpr int MESH_HEIGHT = 5;
constexpr int MESH_WIDTH = 4;
constexpr int PATCH_HEIGHT = 20 * MESH_HEIGHT;
constexpr int PATCH_WIDTH = 20 * MESH_WIDTH;
constexpr double Z_TRANSLATION_SCALE = 1;

const glm::vec3 color = {1., 0, 0};
const glm::vec3 light_position = {0., -5., 0.};

struct MeshNode
{
    glm::vec3 pos = {0, 0, 0};
    float pad = 0;
};

std::vector<MeshNode> mesh_nodes;
std::vector<MeshNode> patch_nodes(PATCH_HEIGHT * PATCH_WIDTH);

static bool init = true;

void update_mesh_nodes()
{
    if (mesh_nodes.empty())
    {
        for (int y = 0; y <= MESH_HEIGHT; ++y)
        {
            for (int x = 0; x <= MESH_WIDTH; ++x)
                mesh_nodes.push_back({{x, 0, y}});
        }
    }

    if (init)
    {
        for (auto& node : mesh_nodes)
        {
            node.pos.y = (node.pos.y  + node.pos.y + Z_TRANSLATION_SCALE * (2 * (double)rand() / INT_MAX - 1)) / 2;
        }
        init = false;
    }
}

GLuint vao_id;
GLuint mesh_node_ssbo_id;
GLuint patch_node_ssbo_id;
GLuint points_ssbo_id;

void display()
{
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mygl::Programs().get_instance()->use(0);
    glm::mat4 world_to_cam_matrix = mygl::Camera::get_instance()->get_world_to_cam_matrix(false);
    glm::mat4 model = glm::mat4(1.0f);
    world_to_cam_matrix = world_to_cam_matrix * model;
    glUniformMatrix4fv(glGetUniformLocation(mygl::Programs::get_instance()->get_id(0), "world_to_cam_matrix"), 1, GL_FALSE, &world_to_cam_matrix[0][0]);
    glUniform3f(glGetUniformLocation(mygl::Programs::get_instance()->get_id(0), "color"), color[0], color[1], color[2]);
    glUniform3f(glGetUniformLocation(mygl::Programs::get_instance()->get_id(0), "light_position"), light_position[0], light_position[1], light_position[2]);

    mygl::Programs().get_instance()->use(1);
    glUniform1i(glGetUniformLocation(mygl::Programs::get_instance()->get_id(1), "MESH_HEIGHT"), MESH_HEIGHT);
    glUniform1i(glGetUniformLocation(mygl::Programs::get_instance()->get_id(1), "MESH_WIDTH"), MESH_WIDTH);
    glUniform1i(glGetUniformLocation(mygl::Programs::get_instance()->get_id(1), "PATCH_HEIGHT"), PATCH_HEIGHT);
    glUniform1i(glGetUniformLocation(mygl::Programs::get_instance()->get_id(1), "PATCH_WIDTH"), PATCH_WIDTH);

    mygl::Programs().get_instance()->use(2);
    glUniform1i(glGetUniformLocation(mygl::Programs::get_instance()->get_id(2), "PATCH_WIDTH"), PATCH_WIDTH);

    // Draw particles

    update_mesh_nodes();

    // Generate Bezier patch

    mygl::Programs().get_instance()->use(1);

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

    std::vector<float> storage(4 * PATCH_WIDTH * PATCH_HEIGHT);
    glGetNamedBufferSubData(patch_node_ssbo_id, 0, 4 * PATCH_WIDTH * PATCH_HEIGHT * sizeof(float), storage.data());

    // Convert Bezier patch to triangles

    mygl::Programs().get_instance()->use(2);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, patch_node_ssbo_id);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, patch_node_ssbo_id);

    glGenBuffers(1, &points_ssbo_id);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, points_ssbo_id);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 2 * 6 * sizeof(struct MeshNode) * patch_nodes.size(), 0, GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, points_ssbo_id);

    glDispatchCompute(PATCH_WIDTH - 1, PATCH_HEIGHT - 1, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    // Display triangles with Phong BRDF

    mygl::Programs().get_instance()->use(0);

    glBindBuffer(GL_ARRAY_BUFFER, points_ssbo_id);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), 0);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)(4 * sizeof(GL_FLOAT)));

    glDrawArrays(GL_TRIANGLES, 0, 6 * PATCH_HEIGHT * PATCH_WIDTH);
    glutSwapBuffers();

    glutPostRedisplay();
}

int main(int argc, char* argv[])
{
    init_context(argc, argv);

    init_display_program("../scenes/bezier_patch/bezier_patch.vs", "../scenes/bezier_patch/bezier_patch.fs");
    init_compute_program("../scenes/bezier_patch/bezier_patch.cs");
    init_compute_program("../scenes/bezier_patch/mesh_creation.cs");

    glutDisplayFunc(display);
    glutMainLoop();

    return 0;
}