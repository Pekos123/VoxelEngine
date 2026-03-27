# GameEngine

A lightweight, modular 2D/3D voxel game engine built with C++20 and OpenGL.

## 🚀 Features

- **Abstraction Layer**: Clean wrappers for OpenGL objects (Vertex Arrays, Buffers, Shaders).
- **Scene Management**: Modular scene system (`e::Scene`) allowing for easy state switching (e.g., Main Menu to Game).
- **Event System**: Lightweight event system (`e::Event`) for decoupled communication between engine components.
- **Voxel Engine**: Efficient chunk-based world system with dynamic loading/unloading.
- **Shadow Mapping**: Directional shadow mapping for the sun with PCF (Percentage Closer Filtering).
- **Fog Rendering**: Linear fog implementation for improved atmosphere and performance masking.
- **Text Rendering**: High-quality text rendering using FreeType.
- **Camera System**: Fully functional 3D camera with mouse and keyboard input handling.
- **Lighting**: Basic lighting implementation with support for object materials and light source visualization.
- **UI Integration**: Real-time parameter tuning using [ImGui](https://github.com/ocornut/imgui).
- **Modern C++**: Built using C++20 standards.

## 🛠 Tech Stack

- **Graphics API**: OpenGL 4.5, GLSL 330
- **Windowing & Input**: [GLFW](https://www.glfw.org/)
- **OpenGL Loader**: [GLAD](https://glad.dav1d.de/)
- **Mathematics**: [GLM](https://github.com/g-truc/glm)
- **Font Rendering**: [FreeType](https://www.freetype.org/)
- **GUI**: [ImGui](https://github.com/ocornut/imgui)
- **Build System**: CMake

## 📦 Getting Started

### Prerequisites

- A C++20 compatible compiler (e.g., MSVC 2019+, GCC 10+, or Clang 10+).
- CMake 3.16 or higher.

### Building

1. Clone the repository:
   ```bash
   git clone https://github.com/Pekos123/VoxelEngine.git
   cd VoxelEngine
   ```

2. Create a build directory and configure:
   ```bash
   mkdir build
   cd build
   cmake ..
   ```

3. Build the project:
   - **Windows**: `cmake --build .`

### Running

After building, you can run the `sandbox` executable located in the build output directory. 

> **Note**: Currently, the sandbox expects shaders to be located relative to the executable path. Ensure the `engine/shaders` directory is accessible.

## 📂 Project Structure

- `engine/`: Core engine library containing the abstraction layer.
  - `include/`: Header files for the engine.
  - `src/`: Implementation of engine components.
  - `shaders/`: Default GLSL shader files (Object, UI, Outline, Shadow).
- `sandbox/`: A demo application demonstrating engine usage.
- `external/`: Third-party dependencies (GLFW, GLM, GLAD, ImGui, OpenSimplex).
- `textures/`: Project-wide texture assets (moved from engine/textures).

## 🎮 Sandbox Controls

- **W/A/S/D**: Move the camera.
- **ESC**: Focus/Unfocues window game.
- **Mouse**: Rotate the camera view.
- **Left Click/Right Click**: Remove/Place blocks.
- **ImGui Windows**: Adjust object colors, light position, and camera sensitivity in real-time.

## 📄 License

This project is open-source and available under the [MIT License](LICENSE).
