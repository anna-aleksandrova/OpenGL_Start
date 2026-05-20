cmake_minimum_required(VERSION 3.10)
project(OpenGL_Start)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(external)

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
set(libs
        ${OPENGL_LIBRARY}
        glfw
        glad
        glm
)
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} ${libs})

add_executable(HW01_Triangles homeworks/hw01/triangles.cpp)
target_link_libraries(HW01_Triangles ${libs})

add_executable(HW02_Interpolation homeworks/hw01/interpolation.cpp)
target_link_libraries(HW02_Interpolation ${libs})