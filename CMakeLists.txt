cmake_minimum_required(VERSION 3.15)
project(DuckEngine VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenGL REQUIRED)

add_executable(DuckEngine
        src/main.cpp
        dependencies/glad.c
        src/engine/core/Engine.cpp
        src/engine/renderer/Shader.cpp
        src/engine/renderer/Texture.cpp
        src/engine/renderer/GBuffer.cpp
        src/engine/renderer/light/LightManager.cpp
        src/engine/renderer/Material.cpp
        src/engine/renderer/Cubemap.cpp
        src/engine/renderer/Skybox.cpp
        src/engine/ecs/Entity.cpp
        src/engine/ecs/Entity.h
        src/engine/ecs/ComponentType.h
        src/engine/ecs/Component.h
        src/engine/ecs/World.cpp
        src/engine/ecs/World.h
        src/engine/system/EntityManager.cpp
        src/engine/system/EntityManager.h
        src/engine/debug/DebugRenderer.cpp
        src/engine/physics/RaycastUtils.h
        src/engine/physics/RaycastUtils.cpp
        src/engine/system/CollisionSystem.h
        src/engine/system/CollisionSystem.cpp
        src/engine/system/DebugRenderSystem.h
        src/engine/system/DebugRenderSystem.cpp
        src/engine/game/DuckEntity.cpp
        src/engine/game/DuckEntity.h
        src/engine/input/InputManager.cpp
        src/engine/game/GameStateManager.cpp
        src/engine/renderer/ShadowMap.cpp
)

# Include directories for headers
target_include_directories(DuckEngine PRIVATE
        ${CMAKE_SOURCE_DIR}/dependencies
        ${CMAKE_SOURCE_DIR}/dependencies/stb_image
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