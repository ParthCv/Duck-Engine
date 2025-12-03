# Duck Engine
Duck Engine is a custom C++ game engine built using OpenGL. It features an Entity-Component-System (ECS) architecture, a deferred rendering pipeline with PBR capabilities, and a dedicated gameplay loop focused on duck shooting mechanics.

Architecture & Systems
The engine is built around five core pillars:

1. Entity Component System (ECS)
   The central design of the engine is the ECS implementation that separates data (Components) from logic (Systems)

World (World.h): The controller of the active scene. It owns the EntityManager, LightManager, and instances of all active Systems. It orchestrates the update logic for every frame.

Entities: Unique identifiers (IDs) that group components together.

Components: Pure data containers (e.g., Transform, Velocity, HealthComponent).

Systems: Logic processors that iterate over entities with specific component signatures.


2. Rendering Pipeline
   The engine uses a modern Deferred Rendering pipeline optimized for many lights and objects:

GBuffer (GBuffer.h): A multi-target framebuffer that stores geometry data (Position, Normal, Albedo, Metallic/Roughness) in textures before lighting is calculated.

PBR & Lighting: Implements Image-Based Lighting (IBL) using Cubemaps (envCubemap, irradianceMap) and a BRDF LUT for realistic material reflections.

Shadow Mapping: Calculates directional shadows using a dedicated shadow framebuffer and the ShadowMap class.

Text Rendering: Uses BitmapFont to render 2D text by parsing font atlases, essential for the dynamic HUD (Score, Timer).

3. Game Logic & Physics
   Procedural Generation: The EnvironmentGenerator dynamically places assets (trees, bushes, rocks) in concentric rings around the player to create a randomized play area.

Game State: The GameStateSystem acts as the high-level arbiter of the game loop, tracking the current round, score, and win/loss conditions via the GameRoundComponent.

Duck AI: A specialized DuckSpawnerManager handles the wave spawning logic, while the MovementSystem and BoundsSystem ensure ducks fly within the play area and reset when out of bounds.

Weapons: The GunSystem handles hitscan shooting via raycasting. It creates a RaycastSource to detect hits and applies visual recoil (pitch and offset) to the camera.

4. Audio & UI
   Audio System: Uses OpenAL for sound playback. The AudioManager handles loading WAV files (via dr_wav) and playing sound effects like gunshots and quacks.

UI Management: The UIManager and UIStateManager handle the 2D overlays. It utilizes LoadingScreen to display texture-based splash screens during asset initialization.

5. Debugging Suite
   Visual Debugging: The engine includes a dedicated DebugRenderer for immediate-mode rendering of lines and shapes.

System Integration: The DebugRenderSystem visualizes invisible logic, drawing wireframes for colliders (BoxCollider) and entity bounds when an entity possesses a DebugDrawable component.

Key Files & Structure
Core (src/engine/core/)
Engine.h/cpp: The main application wrapper. It manages the GLFW window, processes input events, and drives the main game loop (run(), update(), render()).

main.cpp: The entry point. Initializes the Engine with a resolution of 1920x1080 and initiates the shutdown sequence upon closing.

model/ImportedModel.h: Wraps the complexity of loading 3D assets (OBJ/GLTF) and generating the corresponding StaticMesh data for the renderer.

ECS & Systems (src/engine/ecs/)
World.h: The central state container. It holds pointers to the Camera, Shader, and LightManager, effectively acting as the bridge between all systems.

system/RenderingSystem.h/cpp: Iterates over entities with StaticMeshComponent and Transform to draw them to the GBuffer.

system/CollisionSystem.h/cpp: Performs AABB (Axis-Aligned Bounding Box) intersection checks for physics interactions.

system/LifecycleSystem.h/cpp: Manages the lifespan of entities, handling the deletion of objects whose time-to-live has expired.

system/TransformSystem.h/cpp: Responsible for computing the world matrices for all entities, propagating changes from parent to child transforms.

Renderer (src/engine/renderer/)
GBuffer.h: Manages the Framebuffer Object (FBO) configurations and texture attachments for the deferred pass.

Shader.h: A wrapper for compiling GLSL shaders and setting uniform variables (matrices, floats, samplers).

LightManager.h: Manages point lights and directional lights, passing their data to the lighting shader.

BitmapFont.h/cpp: Handles the loading of font texture atlases and character mapping for on-screen text rendering.

Game Specifics (src/engine/game/)
EnvironmentGenerator.h: Uses weighted probability maps (e.g., pine_tree_1.obj: 10.0 weight) to procedurally generate the map layout.

DuckFactory.cpp: A factory pattern implementation for assembling Duck entities with the correct mesh, collider, and velocity components.

ecs/system/GameStateSystem.h/cpp: Controls the flow of the game, managing round timers, score tracking, and state transitions (e.g., from Menu to Gameplay).

ecs/system/DuckDeathSystem.h/cpp: Detects when a duck's HealthComponent reaches zero, triggers death events, and flags the entity for removal.