add_library(engine STATIC
    src/Window.cpp
    src/Renderer.cpp
    src/Shader.cpp
    src/Buffer.cpp
    src/VertexArray.cpp
    src/Application.cpp
    src/Camera.cpp
    src/Utils.cpp
    src/World.cpp
    src/Texture.cpp
    src/Texture2D.cpp
    src/Player.cpp
    src/UIBlockDisplay.cpp

    # GLAD
    ../external/glad/src/glad.c

    # NoiseGen
    ../external/OpenSimplex/OpenSimplex2S.cpp

    # ImGui core
    ../external/imgui/imgui.cpp
    ../external/imgui/imgui_draw.cpp
    ../external/imgui/imgui_tables.cpp
    ../external/imgui/imgui_widgets.cpp
    ../external/imgui/imgui_demo.cpp

    # ImGui backends
    ../external/imgui/backends/imgui_impl_glfw.cpp
    ../external/imgui/backends/imgui_impl_opengl3.cpp
)

# Include directories
target_include_directories(engine PUBLIC
    include
    ../external/glad/include
    ../external # Points to directory containing 'glm', 'imgui'
    ../external/imgui
    ../external/imgui/backends
    ../external/stb
)

# Link libraries
target_link_libraries(engine
    glfw
    opengl32
)

# Define for ImGui OpenGL3 backend
target_compile_definitions(engine PRIVATE
    IMGUI_IMPL_OPENGL_LOADER_GLAD
)