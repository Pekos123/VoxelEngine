# Architecture Overview

This document describes the design and internal structure of the `GameEngine`.

## 🏛 High-Level Structure

The project is divided into two main layers:
1. **Engine Layer (`engine/`)**: A static library that encapsulates OpenGL logic and providing a high-level API for rendering.
2. **Client Layer (`sandbox/`)**: An executable that uses the engine to create a graphical application.

## 🔄 The Application Lifecycle

The engine uses a standard "Application-Loop" pattern:

1. **Inheritance**: The client creates a class that inherits from `e::Application`.
2. **Initialization**: The `e::Application` constructor initializes the `Window` (using GLFW and GLAD).
3. **Loop**: The `Run()` method contains the main engine loop, which calls `OnUpdate()` in every frame.
4. **Shutdown**: Resources are cleaned up in the destructors of the various classes.

## 👤 Player & Physics System
The `e::Player` class handles movement and collision detection:
- **AABB Collision**: Uses Axis-Aligned Bounding Boxes to detect intersections with solid blocks.
- **Physics**: Implements gravity, jumping, and friction-based movement.
- **Collision Resolution**: Collisions are resolved axis-by-axis to allow for smooth sliding along walls and floors.

## 🎨 Rendering Pipeline

### `e::Renderer`
A static class responsible for global rendering state and execution.

### Texture Management
- **`e::TextureArray`**: Used for block textures to enable efficient batching. Shaders use a `layer` index to select the correct texture from the array, avoiding frequent texture swaps.
- **`e::Texture2D`**: Used for standard single-image textures (e.g., UI elements).

### Abstraction Layer
The engine wraps low-level OpenGL objects into clean C++ classes:
- **`Buffer`**: Wraps `VBO` (Vertex Buffer Object) and `EBO` (Element Buffer Object).
- **`VertexArray`**: Wraps `VAO` (Vertex Array Object).
- **`Shader`**: Manages GLSL shaders.

## 🎥 Camera System
The `e::Camera` class handles the view and projection matrices. It supports:
- **Input Handling**: Processes keyboard and mouse movement to update the camera's transform.
- **Math**: Calculates the View-Projection matrix required for vertex shaders.

## 🧱 Voxel Engine System

### Chunks & World
- **`e::World`**: Manages a dynamic set of `e::Chunk` objects. It handles:
    - **Dynamic Loading**: Loads and unloads chunks based on the player's position and render distance.
    - **Persistence**: Supports saving and loading world data to/from binary files.
- **`e::Chunk`**: Represents a 16x128x16 region of blocks.

### Terrain Generation
- **`e::TerrainGenerator`**: Uses **OpenSimplex2S** noise to generate procedural landscapes. It combines continental noise (biomes/mountains) with detail noise for realistic terrain.

### Performance: Data Packing & AO
To maximize rendering efficiency, the engine uses **bit-packing** for vertex data:
- **Memory Optimization**: Vertex data is packed into a single 32-bit integer.
- **Ambient Occlusion (AO)**: During mesh generation, the engine calculates per-vertex AO values based on neighboring blocks, which are also packed into the vertex data.
- **Block ID**: Each vertex carries its block ID, used by the shader to index into the `TextureArray`.

## 🖼 UI System
The `e::UIBlockDisplay` class allows rendering 3D block icons directly onto the 2D screen, typically used for inventories or hotbars. It uses specialized shaders to project a small cube with the correct block textures.

## 🔧 Utilities
`e::Utils` provides common helper functions, such as `ReadFile`, random number generation, and the core logic for **geometry construction** through `addPackedFace`.
