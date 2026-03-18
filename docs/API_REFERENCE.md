# Engine API Reference

A quick reference for the core classes available in the `e` namespace.

---

## 🛠 `e::Application`
The base class for your application. Inherit from this to create your own project.

### Methods
- `virtual void OnUpdate()`: Override this to implement your per-frame logic.
- `void Run()`: Starts the main loop.
- `bool ShouldClose() const`: Checks if the window should close.

---

## 👤 `e::Player`
Manages player state, physics, and collisions.

### Methods
- `Player(glm::vec3 spawnPos)`: Initializes the player.
- `void Update(float deltaTime, World& world)`: Updates position and resolves collisions.
- `void HandleInput(GLFWwindow* window, glm::vec3 camOrientation)`: Processes movement and jumping.
- `Box GetAABB() const`: Returns the player's Axis-Aligned Bounding Box.
- `bool CanPlaceBlock(const glm::ivec3& targetBlockPos) const`: Checks if a block can be placed without intersecting the player.

---

## 🎨 `e::TextureArray` & `e::Texture2D`
Manages OpenGL textures.

### `e::TextureArray` Methods
- `TextureArray(uint32_t width, uint32_t height, uint32_t layers)`: Creates a 2D Texture Array.
- `void AddTexture(const std::string& path, uint32_t layer)`: Loads an image into a specific layer.
- `void Bind(uint32_t slot = 0)`: Binds the texture array to a texture unit.

### `e::Texture2D` Methods
- `Texture2D(const std::string& path)`: Loads a standard 2D texture.
- `void Bind(uint32_t slot = 0)`: Binds the texture.

---

## 🖼 `e::UIBlockDisplay`
Handles rendering 3D block icons in the UI.

### Methods
- `void DrawBlockIcon(uint32_t textureID, int blockId, glm::vec2 screenPos, float size, int screenWidth, int screenHeight)`: Renders a block icon at the specified screen position.

---

## 🖌 `e::Renderer`
A static class for handling global rendering tasks.

### Methods
- `static void Clear()`: Clears the current color and depth buffers.
- `static void SetClearColor(const glm::vec4& color)`: Sets the background color.
- `static void Draw(const std::shared_ptr<VertexArray>& vertexArray, uint32_t vertexCount)`: Executes a draw call for a specified number of vertices.
- `static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray)`: Executes a draw call for the given vertex array using its index buffer.

### Properties
- `static float deltaTime`: Time elapsed since the last frame (useful for framerate-independent logic).

---

## 🏗 `e::VertexArray`
Wraps an OpenGL VAO (Vertex Array Object).

### Methods
- `void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)`: Adds a VBO and sets its layout.
- `void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)`: Sets the EBO/IBO for this vertex array.
- `void Bind()` / `void Unbind()`: Manages the OpenGL binding state.

---

## 📜 `e::Shader`
Wraps an OpenGL shader program.

### Methods
- `Shader(const std::string& vertexSource, const std::string& fragmentSource)`: Compiles and links the shader.
- `void Bind()` / `void Unbind()`: Activates the shader program.
- `void SetUniformMat4(const std::string& name, const glm::mat4& value)`: Sets a 4x4 matrix uniform.
- `void SetUniformFloat3(const std::string& name, const glm::vec3& value)`: Sets a vec3 uniform.

---

## 📷 `e::Camera`
Manages the view and projection matrices and player input.

### Methods
- `Camera(glm::vec3 pos, Window* window)`: Initializes the camera.
- `glm::mat4 GetViewProjectionMatrix(...)`: Calculates and returns the combined VP matrix.
- `void Inputs()`: Processes keyboard and mouse input.

---

## 🌍 `e::World`
Manages chunks, terrain generation, and world-space queries.

### Methods
- `void Update(const glm::vec3& cameraPos, float renderDistance)`: Dynamically loads/unloads chunks.
- `void Draw(const std::shared_ptr<Shader>& shader, ...)`: Renders all chunks within range.
- `uint8_t GetBlock(int x, int y, int z)`: Returns the block type at world coordinates.
- `void SetBlock(int x, int y, int z, uint8_t type)`: Sets a block at world coordinates.
- `void SaveToFile(const std::string& filename)`: Persists the world state to disk.
- `bool LoadFromFile(const std::string& filename)`: Loads a world state from disk.
- `RaycastResult Raycast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance)`: Finds the first solid block in a given direction.

---

## 🧊 `e::Chunk`
A 16x128x16 container for voxel data.

### Properties
- `uint8_t blocks[16][128][16]`: The raw voxel data.
- `glm::ivec3 position`: The world-space origin of this chunk.

---

## 🎯 `e::RaycastResult`
Return type for world raycasting queries.

### Properties
- `bool hit`: True if a block was hit.
- `glm::ivec3 blockPos`: The world coordinates of the hit block.
- `glm::ivec3 normal`: The face normal of the hit side.

---

## 🛠 `e::Utils`
Static helper functions for file I/O and data packing.

### Methods
- `static std::string ReadFile(const std::string& filePath)`: Reads a text file.
- `uint32_t packVertex(x, y, z, ao, face, vIdx, blockID)`: Packs vertex attributes into a 32-bit integer.
- `void addPackedFace(...)`: Generates packed geometry for a single cube face.
