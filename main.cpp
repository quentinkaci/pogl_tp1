#include "init.hh"
#include "matrix4.hh"
#include "program.hh"
#include <iostream>

using namespace mygl;

int main(int argc, char* argv[])
{
    // Initialization

    init_gl();
    initGlut(argc, argv);
    initGlew();

    // Shaders and Program handling

    auto program = program::make_program("FIXME", "FIXME");

    if (program->is_ready())
        program->use();
    else
        std::cerr << "Program error: " << program->get_log() << std::endl;

    return 0;
}