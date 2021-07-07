#include "init.hh"
#include "matrix4.hh"
#include "program.hh"
#include "shaders/shaders.hh"
#include <fstream>
#include <iostream>

using namespace mygl;

int main(int argc, char* argv[])
{
    // Initialization

    if (!init_gl())
        std::cerr << "OpenGL initialization failed" << std::endl;
    if (!initGlut(argc, argv))
        std::cerr << "Glut initialization failed" << std::endl;
    if (!initGlew())
        std::cerr << "Glew initialization failed" << std::endl;

    // Shaders and Program handling

    std::ifstream vertex_shader;
    vertex_shader.open("../shaders/basic.vert");
    if (!vertex_shader.is_open())
    {
        std::cerr << "Vertex shader cannot be opened" << std::endl;
        return 1;
    }
    std::string vertex_shader_content((std::istreambuf_iterator<char>(vertex_shader)),
                                      (std::istreambuf_iterator<char>()));

    std::ifstream fragment_shader;
    fragment_shader.open("../shaders/basic.frag");
    if (!fragment_shader.is_open())
    {
        std::cerr << "Fragment shader cannot be opened" << std::endl;
        return 1;
    }
    std::string fragment_shader_content((std::istreambuf_iterator<char>(fragment_shader)),
                                        (std::istreambuf_iterator<char>()));

    auto program = program::make_program(vertex_shader_content, fragment_shader_content);

    if (program->is_ready())
        program->use();
    else
        std::cerr << "Program error: " << program->get_log() << std::endl;

    // Shaders setup

    setProgram(program);

    initUniformVariables(program);
    initVAO();

    // Display

    glutMainLoop();

    return 0;
}