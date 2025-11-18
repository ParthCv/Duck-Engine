cmake_minimum_required(VERSION 3.15)
project(DuckEngine VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenGL REQUIRED)

add_executable(DuckEngine
        src/main.cpp
        dependencies/glad.c
        src/engine/core/Engine.cpp
        src/engine/renderer/Shader.cpp
        src/engine/input/InputManager.cpp
)

# Include directories for headers
target_include_directories(DuckEngine PRIVATE
        ${CMAKE_SOURCE_DIR}/dependencies
)

# Import GLFW as a static library
add_library(glfw3 STATIC IMPORTED)
set_target_properties(glfw3 PROPERTIES
        IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/dependencies/GLFW/lib-mingw-w64/libglfw3.a"
)

# Link libraries (order matters!)
target_link_libraries(DuckEngine
        glfw3
        OpenGL::GL
        gdi32      # Windows GDI
        user32     # Windows user32
        kernel32   # Windows kernel32
)