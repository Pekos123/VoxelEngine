# Architecture Overview

This document describes the design and internal structure of the `GameEngine`.

## 🏛 High-Level Structure

The project is divided into two main layers:
1. **Engine Layer (`engine/`)**: A static library that encapsulates OpenGL logic and providing a high-level API for rendering.
2. **Client Layer (`sandbox/`)**: An executable that uses the engine to create a graphical application.

## 🔄 The Application Lifecycle

The engine uses a scene-based application loop:

1. **Inheritance**: The client creates a class that inherits from `e::Application`.
2. **Initialization**: The `e::Application` constructor initializes the `Window`.
3. **Scene Management**: The client defines classes inheriting from `e::Scene` (e.g., `MainMenu`, `Game`).
4. **Loop**: The client's `main` function creates an `Application` and manages scene transitions, typically using `e::Event`.
5. **Update**: The current scene's `Update()` method is called every frame.

## 📡 Event System
The `e::Event` class provides a simple way to decouple components:
- **Listeners**: Objects can register member functions as listeners.
- **Invocation**: When an event is invoked, all registered listeners are called.
- **Usage**: Used for scene switching (e.g., clicking "Start" in the menu triggers a scene change event).

## 👤 Player & Physics System
The `e::Player` class handles movement and collision detection:
- **AABB Collision**: Uses Axis-Aligned Bounding Boxes to detect intersections with solid blocks.
- **Physics**: Implements gravity, jumping, and friction-based movement.
- **Collision Resolution**: Collisions are resolved axis-by-axis to allow for smooth sliding along walls and floors.

## 🎨 Rendering Pipeline

### `e::Renderer`
A static class responsible for global rendering state and execution. It also tracks `deltaTime` for frame-independent logic.

### Shadow Mapping
- **`e::ShadowMap`**: Manages a depth framebuffer and texture for directional light (sun).
- **PCF (Percentage Closer Filtering)**: The fragment shader implements 3x3 PCF to soften shadow edges.

### Fog
- **Linear Fog**: Implemented in the fragment shader to fade distant terrain into a background color, enhancing atmosphere and masking chunk loading.

### Texture Management
- **`e::TextureArray`**: Used for block textures to enable efficient batching. Shaders use a `layer` index to select the correct texture from the array.
- **`e::Texture2D`**: Used for standard single-image textures (e.g., UI elements).
- **Global Textures**: Block and crop textures are located in the root `/textures` directory for project-wide accessibility.

### Low-Level Abstractions
The engine wraps low-level OpenGL objects into clean C++ classes:
- **`Buffer`**: Wraps `VBO` (Vertex Buffer Object) and `EBO` (Element Buffer Object).
- **`VertexArray`**: Wraps `VAO` (Vertex Array Object).
- **`Shader`**: Manages GLSL shaders.

## 🎥 Camera System
The `e::Camera` class handles the view and projection matrices. In the current architecture, input handling and movement logic are moved to the `Scene` (e.g., `Game` class) to allow for different camera behaviors in different game states.

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
The engine provides a 2D UI system in the `e::UI` namespace:
- **`e::UI::Rectangle`**: Renders textured or colored 2D rectangles.
- **`e::UI::Text`**: Renders high-quality text using the **FreeType** library. It supports font loading and dynamic color/scale adjustment.
- **`e::UIBlockDisplay`**: (Legacy) Used for rendering 3D block icons.

## 🔧 Utilities
`e::Utils` provides common helper functions, such as `ReadFile`, random number generation, and the core logic for **geometry construction** through `addPackedFace`.
