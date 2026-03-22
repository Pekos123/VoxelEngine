# Contributing to VoxelEngine

## 📜 Simple Rules

1.  **Code Style**:
    *   Use PascalCase for Classes and Methods (e.g., `class VertexArray`, `void Draw()`).
    *   Use camelCase for variables and private members (e.g., `int vertexCount`).
    *   Prefix private members with `m_` (e.g., `uint32_t m_RendererID`).
    *   Use namespaces (everything that is engine based should be under the `e` namespace).

2.  **Modern C++**:
    *   Prefer `std::shared_ptr` and `std::unique_ptr` over raw pointers.
    *   Use `auto` where it improves readability.
    *   Leverage C++20 features (like `std::filesystem`).

3.  **Shaders**:
    *   Keep shaders in `engine/shaders/`.
    *   Ensure any changes to the packed vertex format in C++ are reflected in `obj.vert` and `shadow.vert`.

4.  **Commits**:
    *   Write clear, concise commit messages.
    *   Follow conventional commits if possible (e.g., `feature:`, `fix:`, `refactor:`, `docs:`).

## 🛠 Development Workflow

1.  **Fork** the repository.
2.  **Create a branch** for your feature or fix.
3.  **Implement** your changes and ensure the project builds.
4.  **Submit a Pull Request** with a description of your work.

## 📁 Texture Management
New textures should be placed in `textures/blocks/` or `textures/ui/`. Remember to update `e::BlocksID` in `World.h` and the texture loading logic in the sandbox if adding new block types.
