#include <GL/glew.h>

#include <GL/gl.h>

#include "camera.hh"
#include "init.hh"
#include "obj_loader.hh"
#include "program.hh"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

constexpr float LIGHT_POS[] = {-1.f, 1.f, 2.f, 1.f};

constexpr int NB_CUBES = 2;

constexpr glm::vec3 CUBE_POS[] = {
    glm::vec3(0.0f, -1.0f, -2.0f),
    glm::vec3(3.0f, -2.0f, -6.0f)
};

std::size_t nb_vertices;

void initUniformVariables()
{
    GLint loc = glGetUniformLocation(mygl::Program::get_instance()->get_id(), "light_position");
    if (loc != -1)
        glUniform4f(loc, LIGHT_POS[0], LIGHT_POS[1], LIGHT_POS[2], LIGHT_POS[3]);
}

void initVAO()
{
    GLuint vao_id;
    GLuint vbo_vertex_ids[3];

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    mygl::ObjLoader::load_obj("../scenes/default/cube.obj", vertices, uvs, normals);
    nb_vertices = vertices.size();

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
}

void display()
{
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    for (unsigned int i = 0; i < NB_CUBES; i++)
    {
        // Translate cube
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, CUBE_POS[i]);
        
        // Rotate cube
        float angle = 20.0f * i;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

        // Update world_to_cam_matrix
        glm::mat4 world_to_cam_matrix = mygl::Camera::get_instance()->get_world_to_cam_matrix();
        world_to_cam_matrix = world_to_cam_matrix * model;
        glUniformMatrix4fv(glGetUniformLocation(mygl::Program::get_instance()->get_id(), "world_to_cam_matrix"), 1, GL_FALSE, &world_to_cam_matrix[0][0]);

        glDrawArrays(GL_TRIANGLES, 0, nb_vertices);
    }

    glutSwapBuffers();
}

int main(int argc, char* argv[])
{
    auto program = init(argc, argv, "../scenes/default/basic.vert", "../scenes/default/basic.frag");
    glutDisplayFunc(display);

    initUniformVariables();
    initVAO();

    TextureManager texture_manager(program);
    texture_manager.add_texture("../scenes/default/texture.png", "texture_sampler");

    glutMainLoop();

    return 0;
}