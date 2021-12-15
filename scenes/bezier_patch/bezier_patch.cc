#include <GL/glew.h>

#include "camera.hh"
#include "init.hh"
#include "program.hh"
#include <glm/glm.hpp>
#include <stdlib.h>

float skybox_vertices[] = {
    -1.0f, 1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,

    -1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,

    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,

    -1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, 1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, 1.0f,
    1.0f, -1.0f, 1.0f};

TextureManager texture_manager;

constexpr int MESH_HEIGHT = 5;
constexpr int MESH_WIDTH = 4;
constexpr int PATCH_HEIGHT = 10 * MESH_HEIGHT;
constexpr int PATCH_WIDTH = 10 * MESH_WIDTH;
constexpr double Z_TRANSLATION_SCALE = 1;
constexpr bool PHONG = false;

const glm::vec3 color = {1., 0, 0};
const glm::vec3 light_position = {0., -5., 0.};
const glm::vec3 skeleton_color = {0., 0., 1.};
const glm::vec3 control_points_color = {0., 1., 0.};
const float control_points_scale = 0.05;

enum AnimationType
{
    WAVE = 0,
    RANDOM_INTERP
};
AnimationType animation_type = WAVE;

struct MeshNode
{
    glm::vec3 pos = {0, 0, 0};
    float pad = 0;
};

std::vector<MeshNode> mesh_nodes;
std::vector<MeshNode> mesh_lines;

static uint animation_time = 0;

void update_mesh_nodes()
{
    if (mesh_nodes.empty())
    {
        for (int y = 0; y <= MESH_HEIGHT; ++y)
        {
            for (int x = 0; x <= MESH_WIDTH; ++x)
                mesh_nodes.push_back({{x, Z_TRANSLATION_SCALE * (2 * (double)rand() / INT_MAX - 1), y}});
        }
    }

    if (animation_type == WAVE)
    {
        for (int y = 0; y <= MESH_HEIGHT; ++y)
        {
            for (int x = 0; x <= MESH_WIDTH; ++x)
            {
                int idx = x + (MESH_WIDTH + 1) * y;
                if (y == animation_time)
                    mesh_nodes[idx].pos.y += 1;
                else if (y == animation_time - 1)
                    mesh_nodes[idx].pos.y -= 1;
            }
        }

        if (animation_time++ > MESH_HEIGHT)
            animation_time = 0;
    }
    else if (animation_type == RANDOM_INTERP)
    {
        for (int y = 0; y <= MESH_HEIGHT; ++y)
        {
            for (int x = 0; x <= MESH_WIDTH; ++x)
            {
                int idx = x + (MESH_WIDTH + 1) * y;
                mesh_nodes[idx].pos.y = (mesh_nodes[idx].pos.y + mesh_nodes[idx].pos.y + Z_TRANSLATION_SCALE * (2 * (double)rand() / INT_MAX - 1)) / 2;
            }
        }
    }

    mesh_lines.clear();

    for (int y = 0; y <= MESH_HEIGHT; ++y)
    {
        for (int x = 0; x <= MESH_WIDTH; ++x)
        {
            int idx = x + y * (MESH_WIDTH + 1);

            if (y < MESH_HEIGHT)
            {
                int bottom_idx = x + (y + 1) * (MESH_WIDTH + 1);
                mesh_lines.push_back(mesh_nodes[idx]);
                mesh_lines.push_back(mesh_nodes[bottom_idx]);
            }

            if (x < MESH_WIDTH)
            {
                int right_idx = (x + 1) + y * (MESH_WIDTH + 1);
                mesh_lines.push_back(mesh_nodes[idx]);
                mesh_lines.push_back(mesh_nodes[right_idx]);
            }
        }
    }
}

GLuint vao_id;
GLuint mesh_node_ssbo_id;
GLuint mesh_lines_buf_id;
GLuint patch_node_ssbo_id;
GLuint points_ssbo_id;

static int count = 0;

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
    glUniform1i(glGetUniformLocation(mygl::Programs::get_instance()->get_id(0), "texture_sampler"), 0);
    glUniform1i(glGetUniformLocation(mygl::Programs::get_instance()->get_id(0), "phong"), PHONG);

    mygl::Programs().get_instance()->use(1);
    glUniform1i(glGetUniformLocation(mygl::Programs::get_instance()->get_id(1), "MESH_HEIGHT"), MESH_HEIGHT);
    glUniform1i(glGetUniformLocation(mygl::Programs::get_instance()->get_id(1), "MESH_WIDTH"), MESH_WIDTH);
    glUniform1i(glGetUniformLocation(mygl::Programs::get_instance()->get_id(1), "PATCH_HEIGHT"), PATCH_HEIGHT);
    glUniform1i(glGetUniformLocation(mygl::Programs::get_instance()->get_id(1), "PATCH_WIDTH"), PATCH_WIDTH);

    mygl::Programs().get_instance()->use(2);
    glUniform1i(glGetUniformLocation(mygl::Programs::get_instance()->get_id(2), "PATCH_WIDTH"), PATCH_WIDTH);

    mygl::Programs().get_instance()->use(3);
    glUniform1i(glGetUniformLocation(mygl::Programs::get_instance()->get_id(3), "texture_sampler"), 0);

    mygl::Programs().get_instance()->use(4);
    glUniform3f(glGetUniformLocation(mygl::Programs::get_instance()->get_id(4), "color"), skeleton_color[0], skeleton_color[1], skeleton_color[2]);
    glUniformMatrix4fv(glGetUniformLocation(mygl::Programs::get_instance()->get_id(4), "world_to_cam_matrix"), 1, GL_FALSE, &world_to_cam_matrix[0][0]);

    mygl::Programs().get_instance()->use(5);
    glUniform3f(glGetUniformLocation(mygl::Programs::get_instance()->get_id(5), "color"), control_points_color[0], control_points_color[1], control_points_color[2]);
    glUniform1f(glGetUniformLocation(mygl::Programs::get_instance()->get_id(5), "scale"), control_points_scale);
    glUniformMatrix4fv(glGetUniformLocation(mygl::Programs::get_instance()->get_id(5), "world_to_cam_matrix"), 1, GL_FALSE, &world_to_cam_matrix[0][0]);

    if (!PHONG)
    {
        mygl::Programs().get_instance()->use(3);

        glm::mat4 view = glm::mat4(glm::mat3(mygl::Camera::get_instance()->get_view_matrix()));
        glm::mat4 projection = mygl::Camera::get_instance()->get_projection_matrix();
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 world_to_cam_matrix = projection * view * model;
        glUniformMatrix4fv(glGetUniformLocation(mygl::Programs::get_instance()->get_id(3), "world_to_cam_matrix"), 1, GL_FALSE, &world_to_cam_matrix[0][0]);

        GLuint skybox_vao_id;
        GLuint skybox_vbo_vertex_ids[1];

        glGenVertexArrays(1, &skybox_vao_id);
        glBindVertexArray(skybox_vao_id);

        glGenBuffers(1, &skybox_vbo_vertex_ids[0]);
        glBindBuffer(GL_ARRAY_BUFFER, skybox_vbo_vertex_ids[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), &skybox_vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        texture_manager.bind_cube_map_texture(0);

        glDepthMask(GL_FALSE);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);
    }

    // Update animation
    if (count++ >= 3)
    {
        update_mesh_nodes();
        count = 0;
    }

    // Generate Bezier patch

    mygl::Programs().get_instance()->use(1);

    glGenVertexArrays(1, &vao_id);
    glBindVertexArray(vao_id);

    glDeleteBuffers(1, &mesh_node_ssbo_id);
    glGenBuffers(1, &mesh_node_ssbo_id);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, mesh_node_ssbo_id);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(struct MeshNode) * mesh_nodes.size(), mesh_nodes.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, mesh_node_ssbo_id);

    glDeleteBuffers(1, &patch_node_ssbo_id);
    glGenBuffers(1, &patch_node_ssbo_id);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, patch_node_ssbo_id);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(struct MeshNode) * PATCH_WIDTH * PATCH_HEIGHT, 0, GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, patch_node_ssbo_id);

    glDispatchCompute(PATCH_WIDTH, PATCH_HEIGHT, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    // Convert Bezier patch to triangles

    mygl::Programs().get_instance()->use(2);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, patch_node_ssbo_id);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, patch_node_ssbo_id);

    glDeleteBuffers(1, &points_ssbo_id);
    glGenBuffers(1, &points_ssbo_id);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, points_ssbo_id);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 2 * 6 * sizeof(struct MeshNode) * PATCH_WIDTH * PATCH_HEIGHT, 0, GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, points_ssbo_id);

    glDispatchCompute(PATCH_WIDTH - 1, PATCH_HEIGHT - 1, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    // Display triangles

    mygl::Programs().get_instance()->use(0);

    if (!PHONG)
    {
        world_to_cam_matrix = mygl::Camera::get_instance()->get_world_to_cam_matrix(false);
        model = glm::mat4(1.0f);
        world_to_cam_matrix = world_to_cam_matrix * model;
        auto camera_position = mygl::Camera::get_instance()->get_position();
        glUniformMatrix4fv(glGetUniformLocation(mygl::Programs::get_instance()->get_id(0), "world_to_cam_matrix"), 1, GL_FALSE, &world_to_cam_matrix[0][0]);
        glUniform3fv(glGetUniformLocation(mygl::Programs::get_instance()->get_id(0), "camera_position"), 1, &camera_position[0]);
    }

    glBindBuffer(GL_ARRAY_BUFFER, points_ssbo_id);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), 0);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)(4 * sizeof(GL_FLOAT)));

    texture_manager.bind_cube_map_texture(0);

    glDrawArrays(GL_TRIANGLES, 0, 6 * PATCH_HEIGHT * PATCH_WIDTH);

    // Display skeleton lines

    mygl::Programs().get_instance()->use(4);

    glDeleteBuffers(1, &mesh_lines_buf_id);
    glGenBuffers(1, &mesh_lines_buf_id);
    glBindBuffer(GL_ARRAY_BUFFER, mesh_lines_buf_id);
    glBufferData(GL_ARRAY_BUFFER, mesh_lines.size() * sizeof(struct MeshNode), mesh_lines.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), 0);

    glDrawArrays(GL_LINES, 0, mesh_lines.size());

    // Display control points

    mygl::Programs().get_instance()->use(5);

    glDrawArrays(GL_LINES, 0, mesh_lines.size());

    glutSwapBuffers();
}

int main(int argc, char* argv[])
{
    init_context(argc, argv);

    init_display_program("../scenes/bezier_patch/bezier_patch.vs", "../scenes/bezier_patch/bezier_patch.fs");
    init_compute_program("../scenes/bezier_patch/bezier_patch.cs");
    init_compute_program("../scenes/bezier_patch/mesh_creation.cs");
    init_display_program("../scenes/bezier_patch/skybox.vs", "../scenes/bezier_patch/skybox.fs");
    init_display_program("../scenes/bezier_patch/skeleton_lines.vs", "../scenes/bezier_patch/skeleton_lines.fs");
    init_display_program("../scenes/bezier_patch/control_points.vs", "../scenes/bezier_patch/control_points.fs", "../scenes/bezier_patch/control_points.gs");

    texture_manager.add_cube_map_texture({"../scenes/bezier_patch/skybox/right.png",
                                          "../scenes/bezier_patch/skybox/left.png",
                                          "../scenes/bezier_patch/skybox/top.png",
                                          "../scenes/bezier_patch/skybox/bottom.png",
                                          "../scenes/bezier_patch/skybox/front.png",
                                          "../scenes/bezier_patch/skybox/back.png"});

    glutDisplayFunc(display);
    glutMainLoop();

    return 0;
}