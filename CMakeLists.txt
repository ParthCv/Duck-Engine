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
        src/engine/ecs/system/EntityManager.cpp
        src/engine/ecs/system/EntityManager.h
        src/engine/debug/DebugRenderer.cpp
        src/engine/physics/RaycastUtils.h
        src/engine/physics/RaycastUtils.cpp
        src/engine/ecs/system/CollisionSystem.h
        src/engine/ecs/system/CollisionSystem.cpp
        src/engine/ecs/system/DebugRenderSystem.h
        src/engine/ecs/system/DebugRenderSystem.cpp
        src/engine/ecs/system/TransformSystem.cpp
        src/engine/ecs/system/RenderingSystem.cpp
        src/engine/ecs/system/MovementSystem.cpp
        src/engine/ecs/system/BoundsSystem.cpp
        src/engine/ecs/system/LifecycleSystem.cpp
        src/engine/game/DuckFactory.cpp
        src/engine/core/managers/InputManager.cpp
        src/engine/core/managers/UIStateManager.cpp
        src/engine/core/model/ImportedModel.cpp
        src/engine/core/model/ImportedModel.h
        src/engine/core/model/StaticMesh.cpp
        src/engine/core/model/StaticMesh.h
        src/engine/renderer/ShadowMap.cpp
        src/engine/core/managers/UIManager.cpp
        src/engine/renderer/BitmapFont.cpp
        src/engine/utils/LoadingScreen.cpp
        src/engine/core/managers/ResourceManager.cpp
        src/engine/core/managers/ResourceManager.h
        dependencies/OpenAL/libs/Win64/dr_wav.h
        src/engine/ecs/system/DuckSpawnerManager.cpp
        src/engine/ecs/system/DuckSpawnerManager.h
        src/engine/core/managers/AudioManager.h
        src/engine/core/managers/AudioManager.cpp
        dependencies/OpenAL/libs/Win64/dr_wav.h
        src/engine/ecs/system/DuckSpawnerManager.cpp
        src/engine/ecs/system/DuckSpawnerManager.h
        src/engine/core/managers/GameStateManager.h
        src/engine/core/managers/GameStateManager.cpp
        src/engine/game/ecs/EnvironmentEntity.cpp
        src/engine/game/ecs/EnvironmentEntity.h
        src/engine/game/EnvironmentGenerator.cpp
        src/engine/game/EnvironmentGenerator.h
        src/engine/game/ecs/GunEntity.cpp
        src/engine/game/ecs/GunEntity.h
        src/engine/ecs/system/GunSystem.h
        src/engine/ecs/system/GunSystem.cpp
)


# Include directories for headers
target_include_directories(DuckEngine PRIVATE
        ${CMAKE_SOURCE_DIR}/dependencies
        ${CMAKE_SOURCE_DIR}/dependencies/stb_image
        ${CMAKE_SOURCE_DIR}/dependencies/OpenAL/include
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
        ${CMAKE_SOURCE_DIR}/dependencies/OpenAL/libs/Win64/OpenAL32.lib
        gdi32      # Windows GDI
        user32     # Windows user32
        kernel32   # Windows kernel32
)
add_custom_command(TARGET DuckEngine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/dependencies/OpenAL/libs/Win64/OpenAL32.dll"  # Put the DLL in a /libs folder in your project root
        $<TARGET_FILE_DIR:DuckEngine>/openal32.dll)