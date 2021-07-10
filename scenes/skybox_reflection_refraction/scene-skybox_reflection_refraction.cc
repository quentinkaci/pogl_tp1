#include <GL/glew.h>

#include "camera.hh"
#include "init.hh"
#include "program.hh"
#include <glm/glm.hpp>
#include <obj_loader.hh>

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

void initUniformVariablesSkyBox()
{
    GLint loc = glGetUniformLocation(mygl::Programs::get_instance()->get_id(0), "texture_sampler");
    if (loc != -1)
        glUniform1i(loc, 0);
}

void initUniformVariablesObject()
{
    GLint loc = glGetUniformLocation(mygl::Programs::get_instance()->get_id(1), "texture_sampler");
    if (loc != -1)
        glUniform1i(loc, 0);
}

int initVAOSkybox()
{
    GLuint skybox_vao_id;
    GLuint skybox_vbo_vertex_ids[1];

    glGenVertexArrays(1, &skybox_vao_id);
    glBindVertexArray(skybox_vao_id);

    glGenBuffers(1, &skybox_vbo_vertex_ids[0]);
    glBindBuffer(GL_ARRAY_BUFFER, skybox_vbo_vertex_ids[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), &skybox_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    return 36;
}

int initVAOObject()
{
    GLuint vao_id;
    GLuint vbo_vertex_ids[2];

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    mygl::ObjLoader::load_obj("../scenes/skybox_reflection_refraction/object.obj", vertices, uvs, normals);

    glGenVertexArrays(1, &vao_id);
    glBindVertexArray(vao_id);

    glGenBuffers(2, vbo_vertex_ids);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex_ids[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex_ids[1]);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    return vertices.size();
}

void display()
{
    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Object rendering

    mygl::Programs().get_instance()->use(1);

    glm::mat4 world_to_cam_matrix = mygl::Camera::get_instance()->get_world_to_cam_matrix();
    glm::mat4 model = glm::mat4(1.0f);
    world_to_cam_matrix = world_to_cam_matrix * model;
    auto camera_position = mygl::Camera::get_instance()->get_position();
    glUniformMatrix4fv(glGetUniformLocation(mygl::Programs::get_instance()->get_id(1), "world_to_cam_matrix"), 1, GL_FALSE, &world_to_cam_matrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(mygl::Programs::get_instance()->get_id(1), "camera_position"), 1, GL_FALSE, &camera_position[0]);
    glUniformMatrix4fv(glGetUniformLocation(mygl::Programs::get_instance()->get_id(1), "model"), 1, GL_FALSE, &model[0][0]);

    auto nb_vertices = initVAOObject();
    texture_manager.bind_cube_map_texture(0);
    glDrawArrays(GL_TRIANGLES, 0, nb_vertices);

    // Skybox rendering

    glDepthFunc(GL_LEQUAL);
    mygl::Programs().get_instance()->use(0);

    // Crop translations to keep skybox far from camera
    glm::mat4 view = glm::mat4(glm::mat3(mygl::Camera::get_instance()->get_view_matrix()));
    glm::mat4 projection = mygl::Camera::get_instance()->get_projection_matrix();
    model = glm::mat4(1.0f);
    world_to_cam_matrix = projection * view * model;
    glUniformMatrix4fv(glGetUniformLocation(mygl::Programs::get_instance()->get_id(0), "world_to_cam_matrix"), 1, GL_FALSE, &world_to_cam_matrix[0][0]);

    nb_vertices = initVAOSkybox();
    texture_manager.bind_cube_map_texture(0);
    glDrawArrays(GL_TRIANGLES, 0, nb_vertices);
    glDepthFunc(GL_LESS);

    glutSwapBuffers();
}

int main(int argc, char* argv[])
{
    init_context(argc, argv);

    init_program("../scenes/skybox_reflection_refraction/skybox_shader.vert", "../scenes/skybox_reflection_refraction/skybox_shader.frag");
    mygl::Programs().get_instance()->use(0);
    initUniformVariablesSkyBox();

    init_program("../scenes/skybox_reflection_refraction/object_shader.vert", "../scenes/skybox_reflection_refraction/object_shader.frag");
    mygl::Programs().get_instance()->use(1);
    initUniformVariablesObject();

    texture_manager.add_cube_map_texture({"../scenes/skybox_reflection_refraction/skybox/right.png",
                                          "../scenes/skybox_reflection_refraction/skybox/left.png",
                                          "../scenes/skybox_reflection_refraction/skybox/top.png",
                                          "../scenes/skybox_reflection_refraction/skybox/bottom.png",
                                          "../scenes/skybox_reflection_refraction/skybox/front.png",
                                          "../scenes/skybox_reflection_refraction/skybox/back.png"});

    glutDisplayFunc(display);
    glutMainLoop();

    return 0;
}