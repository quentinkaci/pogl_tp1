cmake_minimum_required(VERSION 3.19)
project(pogl)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)

include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 20)

add_executable(pogl main.cpp)

target_link_libraries(pogl OpenGL::GL GLUT::GLUT GLEW::GLEW)