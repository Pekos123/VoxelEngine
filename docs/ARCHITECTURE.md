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

## 🎨 Rendering Pipeline

### `e::Renderer`
A static class responsible for global rendering state and execution.
- **Clear & Colors**: Manages background clear color and buffer clearing.
- **DrawIndexed**: Takes a `VertexArray` and executes the OpenGL draw call.

### Abstraction Layer
The engine wraps low-level OpenGL objects into clean C++ classes:
- **`Buffer`**: Wraps `VBO` (Vertex Buffer Object) and `EBO` (Element Buffer Object). Supports data layouts for vertex attributes.
- **`VertexArray`**: Wraps `VAO` (Vertex Array Object), managing the relationship between buffers and attribute pointers.
- **`Shader`**: Manages the compilation, linking, and uniform setting of GLSL shaders.

## 🎥 Camera System
The `e::Camera` class handles the view and projection matrices. It supports:
- **Input Handling**: Processes keyboard (WASD) and mouse movement (right-click to rotate) to update the camera's transform.
- **Math**: Uses GLM to calculate the View-Projection matrix required for vertex shaders.

## 🧱 Voxel Engine System

### Chunks & World
The engine implements a chunk-based voxel system:
- **`e::World`**: A manager class that stores loaded chunks in an `unordered_map`. It handles dynamic loading/unloading based on camera distance and manages terrain generation.
- **`e::Chunk`**: Represents a 16x16x16 cube of blocks. Each chunk generates its own mesh only when needed (on load or when a block is modified).

### Performance: Data Packing
To maximize rendering efficiency, the engine uses **bit-packing** for vertex data:
- **Memory Optimization**: Instead of storing floats for positions and normals (36 bytes per vertex), the engine packs all necessary data into a single 32-bit integer (4 bytes).
- **GPU Decoding**: The vertex shader (`obj.vert`) decodes this integer at runtime to reconstruct the geometry.
- **Face Culling**: Faces between adjacent opaque blocks are skipped during mesh generation to reduce the polygon count.

### World Queries: DDA Raycasting
Interaction with blocks is handled through a **Digital Differential Analyzer (DDA)** algorithm:
- **Accuracy**: It ensures every block along the ray path is checked precisely.
- **Normal Detection**: Returns the face normal of the hit block, which is used for correct block placement.

## 🔧 Utilities
`e::Utils` provides common helper functions, such as `ReadFile`, and the core logic for **geometry construction** through `addPackedFace`.
