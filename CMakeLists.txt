cmake_minimum_required(VERSION 3.19)
project(pogl)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
find_package(PNG REQUIRED)
find_package(glm REQUIRED)

link_directories(${GLM_LIBRARY_DIRS})
add_definitions(${GLM_DEFINITIONS})
include_directories(src/
                    ${OPENGL_INCLUDE_DIRS}
                    ${GLUT_INCLUDE_DIRS}
                    ${PNG_INCLUDE_DIR}
                    ${GLM_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 20)

file(GLOB SCENE_SOURCES scenes/**/*.cc)
foreach(scene_source ${SCENE_SOURCES})
    get_filename_component(scene_name ${scene_source} NAME_WE)
    add_executable(${scene_name} ${scene_source})
    target_link_libraries(${scene_name} OpenGL::GL GLUT::GLUT GLEW::GLEW ${PNG_LIBRARY})
endforeach(scene_source ${SCENE_SOURCES})