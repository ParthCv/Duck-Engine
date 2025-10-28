cmake_minimum_required(VERSION 4.0)
project(DuckEngine VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)

add_executable(DuckEngine src/config.h src/main.cpp)

target_include_directories(DuckEngine PRIVATE Dependencies)

add_library(glfw3 STATIC IMPORTED)
set_target_properties(glfw3 PROPERTIES
        IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/Dependencies/GLFW/lib-mingw-w64/libglfw3.a"
)

target_link_libraries(DuckEngine glfw3 opengl32)