#include <GL/glew.h>
#include <GL/glut.h>

bool initGlut(int& argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(1024, 1024);
    glutInitWindowPosition(10, 10);
    glutCreateWindow("Test OpenGL - POGL");
    // FIXME glutDisplayFunc(display);
    return true;
}

bool initGlew()
{
    return (glewInit() == GLEW_OK);
}

bool init_gl()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthRange(0.0, 1.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CCW);
    glClearColor(0.0, 0, 0, 0);

    // FIXME
    return true;
}