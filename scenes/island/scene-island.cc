#include <GL/glew.h>

#include <GL/gl.h>

#include "camera.hh"
#include "init.hh"
#include "obj_loader.hh"
#include "program.hh"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

constexpr float LIGHT_POS[] = {-1.f, 1000.f, 2.f, 1.f};

constexpr int NB_CUBES = 2;

constexpr glm::vec3 CUBE_POS[] = {
    glm::vec3(0.0f, -1.5f, -2.0f),
    glm::vec3(3.0f, -2.0f, -6.0f)};

TextureManager texture_manager;

void initUniformVariables()
{
    GLint loc = glGetUniformLocation(mygl::Program::get_instance()->get_id(), "light_position");
    if (loc != -1)
        glUniform4f(loc, LIGHT_POS[0], LIGHT_POS[1], LIGHT_POS[2], LIGHT_POS[3]);
}

int initVAO(const std::string& obj_path)
{
    GLuint vao_id;
    GLuint vbo_vertex_ids[3];

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    mygl::ObjLoader::load_obj(obj_path, vertices, uvs, normals);

    glGenVertexArrays(1, &vao_id);
    glBindVertexArray(vao_id);

    glGenBuffers(3, vbo_vertex_ids);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex_ids[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex_ids[1]);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex_ids[2]);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    return vertices.size();
}

void display()
{
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 world_to_cam_matrix = mygl::Camera::get_instance()->get_world_to_cam_matrix();
    glm::mat4 model = glm::mat4(1.0f);
    world_to_cam_matrix = world_to_cam_matrix * model;
    glUniformMatrix4fv(glGetUniformLocation(mygl::Program::get_instance()->get_id(), "world_to_cam_matrix"), 1, GL_FALSE, &world_to_cam_matrix[0][0]);

    // Draw water
    texture_manager.bind_texture(0);
    int nb_vertices = initVAO("../scenes/island/water.obj");
    glDrawArrays(GL_TRIANGLES, 0, nb_vertices);

    // Draw sand
    texture_manager.bind_texture(1);
    nb_vertices = initVAO("../scenes/island/sand.obj");
    glDrawArrays(GL_TRIANGLES, 0, nb_vertices);

    // Draw palm
    texture_manager.bind_texture(2);
    nb_vertices = initVAO("../scenes/island/palm.obj");
    glDrawArrays(GL_TRIANGLES, 0, nb_vertices);

    // Draw leaf
    texture_manager.bind_texture(3);
    nb_vertices = initVAO("../scenes/island/leaf.obj");
    glDrawArrays(GL_TRIANGLES, 0, nb_vertices);

    glutSwapBuffers();
}

int main(int argc, char* argv[])
{
    auto program = init(argc, argv, "../scenes/island/basic.vert", "../scenes/island/basic.frag");
    glutDisplayFunc(display);

    initUniformVariables();

    texture_manager.set_program(program);

    texture_manager.add_texture("../scenes/island/water.png");
    texture_manager.add_texture("../scenes/island/sand.png");
    texture_manager.add_texture("../scenes/island/palm.png");
    texture_manager.add_texture("../scenes/island/leaf.png");

    glutMainLoop();

    return 0;
}