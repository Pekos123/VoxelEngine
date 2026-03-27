# Engine API Reference

A quick reference for the core classes available in the `e` namespace.

---

## 🛠 `e::Application`
The base class for your application.

### Methods
- `Application(const std::string& title)`: Initializes the application.
- `virtual void OnUpdate()`: Override this to implement per-frame logic (if not using Scene system).
- `void Run()`: Starts the main loop.
- `void OnWindowUpdate()`: Polls events and swaps buffers.
- `bool ShouldClose() const`: Checks if the window should close.

---

## 📡 `e::Event`
A lightweight event system for decoupled communication.

### Methods
- `template<typename T> void AddListener(T* instance, void (T::*method)())`: Registers a member function as a listener.
- `bool Invoke()`: Calls all registered listeners. Returns true if listeners were present.

---

## 🎭 `e::Scene`
Base class for modular game states.

### Methods
- `virtual void Update()`: Override this to implement scene-specific logic.

---

## 👤 `e::Player`
Manages player state, physics, and collisions.

### Methods
- `Player(glm::vec3 spawnPos)`: Initializes the player.
- `void Update(float deltaTime, World& world)`: Updates position and resolves collisions.
- `void HandleInput(GLFWwindow* window, glm::vec3 camOrientation)`: Processes movement and jumping.
- `Box GetAABB() const`: Returns the player's Axis-Aligned Bounding Box.

---

## 🎨 `e::TextureArray` & `e::Texture2D`
Manages OpenGL textures.

### `e::TextureArray` Methods
- `TextureArray(uint32_t width, uint32_t height, uint32_t layers)`: Creates a 2D Texture Array.
- `void AddTexture(const std::string& path, uint32_t layer)`: Loads an image into a specific layer.
- `void Bind(uint32_t slot = 0)`: Binds the texture array.

### `e::Texture2D` Methods
- `Texture2D(const std::string& path)`: Loads a standard 2D texture.
- `void Bind(uint32_t slot = 0)`: Binds the texture.

---

## 🖼 `e::UI` Namespace
Custom 2D UI components.

### `e::UI::Rectangle`
- `Rectangle(glm::ivec2 size)`: Creates a rectangle of specified size.
- `void SetTexture(const std::string& path)`: Applies a texture.
- `void Draw(int screenWidth, int screenHeight)`: Renders the rectangle.
- `void CompileShaders(const std::string& vertexSrc, const std::string& fragmentSrc)`: Custom shader setup.

### `e::UI::Text`
- `Text()`: Initializes the text renderer.
- `void LoadFont(const std::string& fontPath, unsigned int fontSize)`: Loads a TTF font using FreeType.
- `void Draw(int screenWidth, int screenHeight)`: Renders the `text` string at `pos` with `color` and `scale`.
- `void CompileShaders(const std::string& vertexSrc, const std::string& fragmentSrc)`: Custom shader setup.

---

## 🖌 `e::Renderer`
A static class for handling global rendering tasks.

### Methods
- `static void Clear()`: Clears buffers and updates `deltaTime`.
- `static void SetClearColor(const glm::vec4& color)`: Sets the background color.
- `static void Draw(const std::shared_ptr<VertexArray>& vertexArray, uint32_t vertexCount)`: Executes a draw call.
- `static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray)`: Executes an indexed draw call.

### Properties
- `static float deltaTime`: Time elapsed since the last frame.

---

## 🏗 `e::VertexArray`
Wraps an OpenGL VAO.

### Methods
- `void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)`: Adds a VBO.
- `void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)`: Sets the EBO/IBO.

---

## 📜 `e::Shader`
Wraps an OpenGL shader program.

### Methods
- `Shader(const std::string& vertexSource, const std::string& fragmentSource)`: Compiles and links the shader.
- `void Bind()` / `void Unbind()`: Manages shader state.
- `void SetUniformMat4(const std::string& name, const glm::mat4& value)`: Sets a mat4 uniform.

---

## 📷 `e::Camera`
Manages the view and projection matrices.

### Methods
- `Camera(glm::vec3 pos, Window* window)`: Initializes the camera.
- `glm::mat4 GetViewProjectionMatrix(...)`: Calculates and returns the combined VP matrix.
- `void Inputs()`: Processes camera-specific input (e.g., toggling cursor focus with ESC).

---

## 🌍 `e::World`
Manages chunks, terrain generation, and world-space queries.

### Constants
- `CHUNK_SIZE`: 16 (Width/Depth of a chunk).
- `CHUNK_HEIGHT`: 128 (Height of a chunk).

### Methods
- `void Update(const glm::vec3& cameraPos, float renderDistance)`: Dynamically loads/unloads chunks.
- `void Draw(const std::shared_ptr<Shader>& shader, ...)`: Renders chunks.
- `RaycastResult Raycast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance)`: Finds the first solid block.
