# Duck Engine

A custom C++ game engine featuring a robust Entity-Component-System (ECS) architecture, advanced OpenGL rendering, and a complete duck hunting game implementation.

## Table of Contents
- [Overview](#overview)
- [Engine Architecture](#engine-architecture)
- [Technical Features](#technical-features)
- [Game Implementation](#game-implementation)
- [Build Instructions](#build-instructions)
- [System Requirements](#system-requirements)
- [Project Structure](#project-structure)

## Overview

Duck Engine is a data-driven game engine built from scratch in C++20, showcasing modern game development practices including component-based design, deferred rendering, and physically-based rendering (PBR). The engine demonstrates its capabilities through a fully-featured duck hunting game with advanced graphics, physics, and audio systems.

## Engine Architecture

### Entity-Component-System (ECS)

The engine implements a complete ECS architecture that promotes modularity, reusability, and performance:

**Core ECS Classes:**
- `World` - Central hub managing all entities and systems
- `Entity` - Lightweight game object container
- `Component` - Pure data structures for entity attributes
- `EntityManager` - Handles entity lifecycle and queries

**Component Types:**
- `Transform` - Position, rotation, and scale
- `Velocity` - Movement and physics data
- `StaticMeshComponent` - 3D model rendering
- `BoxCollider` - Collision detection volumes
- `BoundsComponent` - Spatial boundaries
- `DuckComponent` - Duck-specific behavior data
- `GunComponent` - Weapon system data
- `HealthComponent` - Damage and health tracking
- `ScoreValueComponent` - Point system integration
- `RaycastSource` - Ray-based interaction
- `DebugDrawable` - Visual debugging

**System Implementations:**
The engine features specialized systems that operate on component data:

| System | Purpose | Components Used |
|--------|---------|-----------------|
| `RenderingSystem` | 3D mesh rendering and material handling | Transform, StaticMesh |
| `MovementSystem` | Physics-based movement | Transform, Velocity |
| `CollisionSystem` | AABB collision detection and response | Transform, BoxCollider |
| `TransformSystem` | Hierarchical transformations | Transform |
| `BoundsSystem` | Spatial boundary checking | Transform, Bounds |
| `LifecycleSystem` | Entity creation and destruction | Health, Lifecycle |
| `DebugRenderSystem` | Visual debugging overlays | DebugDrawable |
| `GunSystem` | Weapon mechanics and shooting | Gun, Transform |
| `DuckSpawnerManager` | Wave-based enemy spawning | Duck, Transform |

### Manager Systems

The engine employs specialized managers for cross-cutting concerns:

- **ResourceManager** - Asset loading, caching, and lifecycle management
- **InputManager** - Keyboard, mouse, and controller input handling
- **AudioManager** - 3D positional audio using OpenAL
- **UIManager** - Immediate-mode UI rendering system
- **UIStateManager** - Game state and menu management
- **GameStateManager** - High-level game flow control

## Technical Features

### Advanced Rendering Pipeline

**Deferred Rendering:**
- G-Buffer implementation for efficient multi-light scenes
- Separate geometry and lighting passes
- Support for multiple light types (directional, point, spot)

**Physically-Based Rendering (PBR):**
- Metallic-roughness workflow
- Image-Based Lighting (IBL)
- Irradiance mapping for diffuse
- Prefiltered environment maps for specular
- BRDF lookup table integration
- HDR environment mapping

**Lighting System:**
- Dynamic shadow mapping
- Directional shadows for sun/moon
- Configurable shadow quality
- Multiple light source support via `LightManager`

**Additional Graphics Features:**
- Skybox rendering with cubemaps
- Model loading and rendering
- Texture management (diffuse, normal, metallic, roughness, AO)
- Custom shader system
- Debug visualization tools

### Physics & Collision

- **Raycasting** - Precise hit detection for shooting mechanics
- **AABB Collision** - Efficient broad-phase collision detection
- **Bounds Checking** - Spatial constraints for game area
- **Physics Integration** - Velocity-based movement with collision response

### Audio System

- **OpenAL Integration** - Hardware-accelerated 3D audio
- **Spatial Audio** - Position-based sound attenuation
- **Sound Management** - Loading, playback, and cleanup
- **Event-Driven** - Sound triggers for game events (shots, hits, background)

### Input & UI

- **Input Manager** - Polling-based input with state tracking
- **Cursor Control** - Show/hide cursor for different game states
- **UI System** - Custom bitmap font rendering
- **State Machine** - Menu, gameplay, pause, game over states
- **Loading Screen** - Asynchronous asset loading with progress display

## Game Implementation

### Duck Hunt Game

The engine's capabilities are demonstrated through a complete duck hunting game:

**Core Gameplay:**
- First-person shooting mechanics with gun recoil
- Wave-based duck spawning with increasing difficulty
- Score system with different duck point values
- Health/lives system with game over conditions
- Dynamic environment generation

**Game Features:**
- **DuckFactory** - Procedural duck entity creation with variants
- **EnvironmentGenerator** - Dynamic background and obstacle placement
- **Gun System** - Raycast-based shooting with visual feedback
- **Spawner System** - Time-based wave management
- **Score Tracking** - Real-time UI updates
- **Sound Effects** - Shooting, hitting, flapping, win/lose audio

**Polish & UX:**
- Loading screen during asset initialization
- Multiple UI states (menu, playing, paused, game over)
- Visual feedback for hits and misses
- Background music and ambient sounds
- Responsive controls and camera movement

## Build Instructions

### Prerequisites

- CMake 3.15 or higher
- C++20 compatible compiler (GCC/MinGW on Windows)
- OpenGL 4.0+ compatible graphics card

### Building

```bash
# Clone the repository
git clone [repository-url]
cd Duck-Engine

# Create build directory
mkdir build
cd build

# Generate build files
cmake ..

# Build the project
cmake --build .
```

### Running

```bash
# From the build directory
./DuckEngine
```

The executable expects the `assets` folder to be in the working directory.

## System Requirements

**Minimum:**
- OS: Windows 10/11
- GPU: OpenGL 4.0+ support
- RAM: 4GB
- Storage: 500MB

**Recommended:**
- OS: Windows 10/11
- GPU: Dedicated graphics card with OpenGL 4.5+
- RAM: 8GB
- Storage: 1GB

## Project Structure

```
Duck-Engine/
├── src/
│   ├── main.cpp                    # Entry point
│   └── engine/
│       ├── core/
│       │   ├── Engine.cpp/h        # Main engine class
│       │   ├── managers/           # Manager systems
│       │   └── model/              # Model loading
│       ├── ecs/
│       │   ├── World.cpp/h         # ECS world
│       │   ├── Entity.cpp/h        # Entity class
│       │   ├── Component.h         # Component base
│       │   ├── components/         # Component definitions
│       │   └── system/             # System implementations
│       ├── renderer/
│       │   ├── Shader.cpp/h        # Shader management
│       │   ├── Texture.cpp/h       # Texture loading
│       │   ├── GBuffer.cpp/h       # Deferred rendering
│       │   ├── ShadowMap.cpp/h     # Shadow mapping
│       │   ├── Skybox.cpp/h        # Skybox rendering
│       │   ├── Material.cpp/h      # Material system
│       │   └── light/              # Lighting system
│       ├── physics/
│       │   └── RaycastUtils.cpp/h  # Raycasting utilities
│       ├── game/
│       │   ├── DuckFactory.cpp/h   # Duck entity creation
│       │   ├── EnvironmentGenerator.cpp/h
│       │   └── ecs/                # Game-specific entities
│       ├── debug/
│       │   └── DebugRenderer.cpp/h # Debug visualization
│       └── utils/
│           └── LoadingScreen.cpp/h # Asset loading UI
├── assets/
│   ├── shaders/                    # GLSL shaders
│   ├── models/                     # 3D models
│   ├── textures/                   # Texture maps
│   ├── audio/                      # Sound files
│   ├── fonts/                      # Bitmap fonts
│   └── sprites/                    # 2D images
├── dependencies/                   # Third-party libraries
├── docs/                          # Design documentation
└── CMakeLists.txt                 # Build configuration
```

## Technical Highlights

### Design Quality
- **Separation of Concerns** - Clear boundaries between engine and game code
- **Data-Driven Design** - Components are pure data, systems contain logic
- **RAII Principles** - Proper resource management throughout
- **Const Correctness** - Immutability where appropriate
- **Modern C++** - Leveraging C++20 features for clarity and performance

### Modularity
- **Component Reusability** - Components can be mixed and matched on entities
- **System Independence** - Systems operate on component data without tight coupling
- **Manager Abstraction** - High-level systems decoupled from implementation details
- **Asset Pipeline** - Centralized resource loading and management

### Extensibility
- **Easy Component Addition** - New components integrate seamlessly
- **System Registration** - New systems can be added to the World
- **Shader Flexibility** - Material system supports custom shaders
- **Event System** - Game events can trigger responses across systems

### Performance Considerations
- **Deferred Rendering** - Efficient handling of multiple lights
- **Spatial Partitioning** - Bounds system for efficient queries
- **Resource Caching** - Prevent redundant asset loading
- **Frame-Based Updates** - Delta time for consistent gameplay

## Code Quality

### Stability
- Proper error handling for file I/O and OpenGL operations
- Safe resource cleanup on shutdown
- Null checks for pointer operations
- Graceful handling of missing assets

### Maintainability
- Clear naming conventions throughout
- Logical file organization
- Consistent code formatting
- Minimal code duplication through reusable systems

### Documentation
- Design documentation in `/docs`
- Clear component and system responsibilities
- Self-documenting code structure
- This comprehensive README

## Future Enhancements

Potential areas for expansion:
- Particle system for visual effects
- Animation system for skeletal meshes
- Serialization for save/load functionality
- Network multiplayer support
- Editor tools for level design
- Additional game modes

## License

[Add your license information here]

## Credits

Developed as a game engine architecture project demonstrating ECS design patterns, advanced rendering techniques, and complete game implementation.

### Third-Party Libraries
- **GLFW** - Window and input management
- **GLAD** - OpenGL loader
- **stb_image** - Image loading
- **OpenAL** - Audio playback
- **Assimp** (implied) - Model loading
