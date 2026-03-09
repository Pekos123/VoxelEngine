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

## 🖌 `e::Renderer`
A static class for handling global rendering tasks.

### Methods
- `static void Clear()`: Clears the current color and depth buffers.
- `static void SetClearColor(const glm::vec4& color)`: Sets the background color.
- `static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray)`: Executes a draw call for the given vertex array.

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
- `Camera(int width, int height, glm::vec3 position)`: Initializes the camera.
- `glm::mat4 GetViewProjectionMatrix(...)`: Calculates and returns the combined VP matrix.
- `void Inputs(Window* window)`: Processes keyboard and mouse input (WASD + mouse rotation).

---

## 🌍 `e::World`
Manages chunks, terrain generation, and world-space queries.

### Methods
- `void Update(const glm::vec3& cameraPos, float renderDistance)`: Dynamically loads/unloads chunks based on camera position.
- `void Draw(const std::shared_ptr<Shader>& shader, ...)`: Renders all chunks within range.
- `uint8_t GetBlock(int x, int y, int z)`: Returns the block type at world coordinates.
- `void SetBlock(int x, int y, int z, uint8_t type)`: Sets a block at world coordinates and updates the mesh.
- `RaycastResult Raycast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance)`: Finds the first solid block in a given direction.

---

## 🧊 `e::Chunk`
A 16x16x16 container for voxel data.

### Properties
- `uint8_t blocks[16][16][16]`: The raw voxel data.
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
- `uint32_t packVertex(...)`: Packs vertex attributes into a single 32-bit integer.
- `void addPackedFace(...)`: Generates packed geometry for a single cube face.
