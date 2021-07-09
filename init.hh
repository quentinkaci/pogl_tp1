#pragma once

#include <GL/glew.h>
#include <GL/glut.h>

#include "camera.hh"
#include "shaders/shaders.hh"

void key_callback(int key, int, int)
{
    mygl::camera::get_instance()->key_callback(key, 0, 0);
}

void mouse_motion_callback(int x, int y)
{
    mygl::camera::get_instance()->mouse_motion_callback(x, y);
}

void mouse_callback(int button, int state, int x, int y)
{
    mygl::camera::get_instance()->mouse_callback(button, state, x, y);
}

inline bool initGlut(int& argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(1024, 1024);
    glutInitWindowPosition(10, 10);
    glutCreateWindow("Test OpenGL - POGL");
    glutDisplayFunc(display);

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