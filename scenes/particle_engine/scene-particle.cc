#include <GL/glew.h>

#include "camera.hh"
#include "init.hh"
#include "program.hh"
#include <glm/glm.hpp>

constexpr int NB_PARTICLES = 100;

struct particle
{
    glm::vec3 pos;
    float size;
    glm::vec3 pad;
    int color_index;
};

GLuint vao_id;
GLuint particle_ssbo_id;
bool init = true;

void display()
{
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_PROGRAM_POINT_SIZE);

    glm::mat4 world_to_cam_matrix = mygl::Camera::get_instance()->get_world_to_cam_matrix(false);
    glm::mat4 model = glm::mat4(1.0f);
    world_to_cam_matrix = world_to_cam_matrix * model;
    glUniformMatrix4fv(glGetUniformLocation(mygl::Programs::get_instance()->get_id(0), "world_to_cam_matrix"), 1, GL_FALSE, &world_to_cam_matrix[0][0]);

    // Draw particles

    mygl::Programs().get_instance()->use(1);

    if (init)
    {
        glGenVertexArrays(1, &vao_id);
        glBindVertexArray(vao_id);

        glGenBuffers(1, &particle_ssbo_id);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, particle_ssbo_id);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(struct particle) * NB_PARTICLES, 0, GL_DYNAMIC_COPY);

        init = false;
    }

    glBindVertexArray(vao_id);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, particle_ssbo_id);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, particle_ssbo_id);

    glDispatchCompute(NB_PARTICLES, 1, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    mygl::Programs().get_instance()->use(0);

    glBindBuffer(GL_ARRAY_BUFFER, particle_ssbo_id);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT) + 1 * sizeof(GL_INT), 0);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_INT, GL_FALSE, 7 * sizeof(GL_FLOAT) + 1 * sizeof(GL_INT), (GLvoid*)(7 * sizeof(GL_FLOAT)));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT) + 1 * sizeof(GL_INT), (GLvoid*)(3 * sizeof(GL_FLOAT)));

    glDrawArrays(GL_POINTS, 0, NB_PARTICLES);
    glutSwapBuffers();

    glutPostRedisplay();
}

int main(int argc, char* argv[])
{
    init_context(argc, argv);

    init_display_program("../scenes/particle_engine/particle.vs", "../scenes/particle_engine/particle.fs");
    init_compute_program("../scenes/particle_engine/particle.cs");

    glutDisplayFunc(display);
    glutMainLoop();

    return 0;
}