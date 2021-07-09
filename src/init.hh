#pragma once

#include <GL/glew.h>
#include <GL/glut.h>
#include <fstream>
#include <iostream>

#include "camera.hh"
#include "program.hh"
#include "texture_manager.hh"

void key_callback(int key, int, int)
{
    mygl::Camera::get_instance()->key_callback(key, 0, 0);
}

void mouse_motion_callback(int x, int y)
{
    mygl::Camera::get_instance()->mouse_motion_callback(x, y);
}

void mouse_callback(int button, int state, int x, int y)
{
    mygl::Camera::get_instance()->mouse_callback(button, state, x, y);
}

inline bool initGlut(int& argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(1024, 1024);
    glutInitWindowPosition(10, 10);
    glutCreateWindow("OpenGL Project");

    glutSpecialFunc(key_callback);
    glutMotionFunc(mouse_motion_callback);
    glutMouseFunc(mouse_callback);

    return true;
}

inline bool initGlew()
{
    return (glewInit() == GLEW_OK);
}

inline bool init_gl()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthRange(0.0, 1.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glClearColor(0.0, 0, 0, 0);

    return true;
}

std::shared_ptr<mygl::Program> init(int argc,
                                    char* argv[],
                                    const std::string& vert_shader_path,
                                    const std::string& frag_shader_path)
{
    // Initialization

    if (!initGlut(argc, argv))
        std::cerr << "Glut initialization failed" << std::endl;
    if (!init_gl())
        std::cerr << "OpenGL initialization failed" << std::endl;
    if (!initGlew())
        std::cerr << "Glew initialization failed" << std::endl;

    // Shaders and Program handling

    std::ifstream vertex_shader;
    vertex_shader.open(vert_shader_path);
    if (!vertex_shader.is_open())
    {
        std::cerr << "Vertex shader cannot be opened" << std::endl;
        exit(1);
    }
    std::string vertex_shader_content((std::istreambuf_iterator<char>(vertex_shader)),
                                      (std::istreambuf_iterator<char>()));

    std::ifstream fragment_shader;
    fragment_shader.open(frag_shader_path);
    if (!fragment_shader.is_open())
    {
        std::cerr << "Fragment shader cannot be opened" << std::endl;
        exit(1);
    }
    std::string fragment_shader_content((std::istreambuf_iterator<char>(fragment_shader)),
                                        (std::istreambuf_iterator<char>()));

    auto program = mygl::Program::make_program(vertex_shader_content, fragment_shader_content);

    if (program->is_ready())
        program->use();
    else
        std::cerr << "Program error: " << program->get_log() << std::endl;

    return program;
}