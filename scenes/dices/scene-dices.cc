#include <GL/glew.h>

#include "camera.hh"
#include "init.hh"
#include "obj_loader.hh"
#include "program.hh"
#include <glm/glm.hpp>

// constexpr float LIGHT_POS[] = {10.f, 10.f, 10.f, 1.f};
constexpr glm::vec3 LIGHT_POS = glm::vec3(0.f, 2.f, 2.f);

GLuint FramebufferName = 0;
GLuint depthTexture;
TextureManager texture_manager;

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
    GLuint depthProgramID = mygl::Programs().get_instance()->get_id(0);
    GLuint programID = mygl::Programs().get_instance()->get_id(1);

    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
    glViewport(0, 0, 1920, 1920);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mygl::Programs().get_instance()->use(0);

    glm::mat4 depthProjectionMatrix = glm::ortho<float>(-10, 10, -10, 10, -10, 20);
	glm::mat4 depthViewMatrix = glm::lookAt(LIGHT_POS, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glm::mat4 depthModelMatrix = glm::mat4(1.0);
	glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;

    GLuint depthMatrixID = glGetUniformLocation(depthProgramID, "depthMVP");
    glUniformMatrix4fv(depthMatrixID, 1, GL_FALSE, &depthMVP[0][0]);

    int nb_vertices = initVAO("../scenes/dices/walls.obj");
    glDrawArrays(GL_TRIANGLES, 0, nb_vertices);

    nb_vertices = initVAO("../scenes/dices/cube.obj");
    glDrawArrays(GL_TRIANGLES, 0, nb_vertices);

    nb_vertices = initVAO("../scenes/dices/table.obj");
    glDrawArrays(GL_TRIANGLES, 0, nb_vertices);

    // ---

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, 1920, 1920);
    glDisable(GL_CULL_FACE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mygl::Programs().get_instance()->use(1);

    glm::mat4 VP = mygl::Camera::get_instance()->get_world_to_cam_matrix(true);
    
    glm::mat4 biasMatrix(
			0.5, 0.0, 0.0, 0.0, 
			0.0, 0.5, 0.0, 0.0,
			0.0, 0.0, 0.5, 0.0,
			0.5, 0.5, 0.5, 1.0
	);
    glm::mat4 depthBiasMVP = biasMatrix*depthMVP;
    GLuint DepthBiasID = glGetUniformLocation(programID, "DepthBiasMVP");
    glUniformMatrix4fv(DepthBiasID, 1, GL_FALSE, &depthBiasMVP[0][0]);

    GLuint ShadowMapID  = glGetUniformLocation(programID, "shadowMap");
    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glUniform1i(ShadowMapID, 10);

    // Draw walls
    // glBindTexture(GL_TEXTURE_2D, 0);
    glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(30, 30, 30));
    model = glm::translate(model, glm::vec3(0, 0.24, 0));
    glm::mat4 MVP = VP * model;
    glUniformMatrix4fv(glGetUniformLocation(programID, "MVP"), 1, GL_FALSE, &MVP[0][0]);
    nb_vertices = initVAO("../scenes/dices/walls.obj");
    glDrawArrays(GL_TRIANGLES, 0, nb_vertices);

    // Draw cube
    texture_manager.bind_texture(0);
    model = glm::scale(glm::mat4(1.0f), glm::vec3(0.45, 0.45, 0.45));
    MVP = VP * glm::translate(model, glm::vec3(0, -5.4, 0));
    glUniformMatrix4fv(glGetUniformLocation(programID, "MVP"), 1, GL_FALSE, &MVP[0][0]);
    nb_vertices = initVAO("../scenes/dices/cube.obj");
    glDrawArrays(GL_TRIANGLES, 0, nb_vertices);

    // Draw table
    texture_manager.bind_texture(1);
    MVP = VP * glm::translate(glm::mat4(1.0f), glm::vec3(0, -10, 0));
    glUniformMatrix4fv(glGetUniformLocation(programID, "MVP"), 1, GL_FALSE, &MVP[0][0]);
    nb_vertices = initVAO("../scenes/dices/table.obj");
    glDrawArrays(GL_TRIANGLES, 0, nb_vertices);

    glutSwapBuffers();
}

int main(int argc, char* argv[])
{
    init_context(argc, argv);

    init_program("../scenes/dices/depth.vert", "../scenes/dices/depth.frag");
    
    // Create depth texture
	glGenFramebuffers(1, &FramebufferName);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT16, 1920, 1920, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);
	glDrawBuffer(GL_NONE);
    
    init_program("../scenes/dices/basic.vert", "../scenes/dices/basic.frag");

    texture_manager.set_program(mygl::Programs().get_instance()->get_id(1));
    texture_manager.add_texture("../scenes/dices/texture.png");
    texture_manager.add_texture("../scenes/dices/table.png");

    glutDisplayFunc(display);
    glutMainLoop();

    return 0;
}