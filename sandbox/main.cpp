#include <Application.h>
#include <Renderer.h>
#include <VertexArray.h>
#include <Shader.h>
#include <Camera.h>
#include <World.h>
#include <Utils.h>
#include <Texture.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <filesystem>
#include <iostream>

class Sandbox : public e::Application
{
    e::Camera camera;
    e::World* world = new e::World(55555); // seed

    std::shared_ptr<e::Shader> objShader;
    std::shared_ptr<e::Shader> outlineShader;

    glm::vec3 lightPos = { 10.0f, 20.0f, 10.0f };
    glm::vec3 objectColor = { 1.f, 1.f, 1.0f }; // White bc of textures
    glm::vec3 skyColor = {0.7f, 0.7f, 0.95f};
    // Outline
    glm::vec3 outlineColor = { 0.0f, 0.0f, 0.0f }; // Black
    float outlineThickness = 2.0f;
    std::unique_ptr<e::VertexArray> outlineVAO;
    std::shared_ptr<e::VertexBuffer> vbo;
    std::shared_ptr<e::IndexBuffer> ib;

    std::unique_ptr<e::TextureArray> texArray;
    int currentPlacingBlockId = 1;

    float renderDistance = 120.0f;

    void RenderCrosshair()
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 center = viewport->GetCenter();
        ImDrawList* draw_list = ImGui::GetForegroundDrawList();

        float crosshairSize = 10.0f;
        float thickness = 2.0f;
        ImU32 color = IM_COL32(255, 255, 255, 200); // White with some transparency

        // Horizontal line
        draw_list->AddLine(
            ImVec2(center.x - crosshairSize, center.y),
            ImVec2(center.x + crosshairSize, center.y),
            color, thickness
        );
        // Vertical line
        draw_list->AddLine(
            ImVec2(center.x, center.y - crosshairSize),
            ImVec2(center.x, center.y + crosshairSize),
            color, thickness
        );
    }

    void DebugWindowRender()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        RenderCrosshair();

        {
            ImGui::Begin("Debug Window");
                ImGui::Text("Chunks: %d", 0); // Placeholder or add world.GetChunkCount()
                ImGui::InputFloat("Sensivity", &camera.sensivity, 0.1f, 0.5f);
                ImGui::InputFloat("Camera Speed", &camera.speed, 0.1f);
                ImGui::SliderFloat("Render Distance", &renderDistance, 32.0f, 800.0f);
                ImGui::ColorEdit3("Object Color", glm::value_ptr(objectColor));
                ImGui::ColorEdit3("Sky Color", glm::value_ptr(skyColor));
                ImGui::DragFloat3("Light Position", glm::value_ptr(lightPos), 0.5f);
                ImGui::ColorEdit3("Outline Color", glm::value_ptr(outlineColor));
                ImGui::DragFloat("Outline Thickness", &outlineThickness, 0.1f, 0.1f, 5.0f);
                ImGui::DragInt("Block Id", &currentPlacingBlockId, 1, 1, 8);   
            ImGui::End();

            // FPS Counter
            ImGui::Begin("Performance");
                ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
                
                e::RaycastResult result = world->Raycast(camera.position, camera.orientation, 10.0f);
                if (result.hit) {
                    ImGui::Text("Target Block: %d, %d, %d", result.blockPos.x, result.blockPos.y, result.blockPos.z);
                } else {
                    ImGui::Text("Target Block: None");
                }
                ImGui::Text("Renderer chunks: %d", world->GetLoadedChunkCount());
            ImGui::End();

        }
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void DebugWindowInit()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGui::StyleColorsDark();
        
        ImGui_ImplGlfw_InitForOpenGL(m_Window->GetGLFWwindow(), true);
        ImGui_ImplOpenGL3_Init("#version 330");
    }

    void DebugWindowShutdown()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void SetupOutlineBuffer()
    {
        float cubeVertices[] = {
            0,0,0,  1,0,0,  1,1,0,  0,1,0,
            0,0,1,  1,0,1,  1,1,1,  0,1,1
        };

        // 12 lines (24 indices)
        unsigned int cubeIndices[] = {
            0,1, 1,2, 2,3, 3,0, // Bottom
            4,5, 5,6, 6,7, 7,4, // Top
            0,4, 1,5, 2,6, 3,7  // Pillars
        };

        outlineVAO = std::make_unique<e::VertexArray>();
        vbo = std::make_shared<e::VertexBuffer>(cubeVertices, sizeof(cubeVertices));
        vbo->SetLayout({
            { e::ShaderDataType::Float3, "aPos" }
        });
        ib = std::make_shared<e::IndexBuffer>(cubeIndices, sizeof(cubeIndices) / sizeof(unsigned));
        
        outlineVAO->AddVertexBuffer(vbo);
        outlineVAO->SetIndexBuffer(ib);
        
    }
    void LoadShaders()
    {
        std::string vPath = (e::Utils::GetRootDir() / "engine/shaders/obj/obj.vert").string();
        std::string fPath = (e::Utils::GetRootDir() / "engine/shaders/obj/obj.frag").string();
        std::string vSrc = e::Utils::ReadFile(vPath);
        std::string fSrc = e::Utils::ReadFile(fPath);

        std::string outlineVPath = (e::Utils::GetRootDir() / "engine/shaders/outline/outline.vert").string();
        std::string outlineFPath = (e::Utils::GetRootDir() / "engine/shaders/outline/outline.frag").string();
        
        std::string outlineFSrc = e::Utils::ReadFile(outlineFPath);
        std::string outlineVSrc = e::Utils::ReadFile(outlineVPath); // Using same source for vert and frag

        if (vSrc.empty() || fSrc.empty()) {
            std::cerr << "CRITICAL: Shader source is empty!" << std::endl;
            return;
        }

        objShader = std::make_shared<e::Shader>(vSrc, fSrc);
        outlineShader = std::make_shared<e::Shader>(outlineVSrc, outlineFSrc); // Using same source for vert and frag
    }

    // draw outline around targeted block
    void DrawOutline()
    {
        e::RaycastResult result = world->Raycast(camera.position, camera.orientation, 10.0f);
        if(result.hit)
        {
            glDepthMask(GL_FALSE); // Don't write to depth, just read
            glDepthFunc(GL_LEQUAL); // "Less than or Equal" is key for ties
            glLineWidth(outlineThickness);

            // Give the lines a "depth priority"
            glEnable(GL_POLYGON_OFFSET_LINE);
            glPolygonOffset(-1.0f, -1.0f);
            
            outlineShader->Bind();
            outlineShader->SetUniformMat4("u_ViewProj", camera.GetViewProjectionMatrix(45.0f, 0.1f, 1000.0f));
            outlineShader->SetUniformFloat3("blockPos", glm::vec3(result.blockPos));
            outlineShader->SetUniformFloat4("outlineColor", {outlineColor, 1.0f});

            outlineVAO->Bind();

            glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, nullptr);

            glDisable(GL_POLYGON_OFFSET_LINE);
            glDepthFunc(GL_LESS);
            glDepthMask(GL_TRUE);
            glLineWidth(1.0f); // Back to normal
        }
    }

    bool leftMouseDown = false;
    bool rightMouseDown = false;
    void Input()
    {
        if (ImGui::GetIO().WantCaptureMouse) return;

        // Left Click: Remove Block
        if(glfwGetMouseButton(m_Window->GetGLFWwindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {
            if (!leftMouseDown) {
                e::RaycastResult result = world->Raycast(camera.position, camera.orientation, 10.0f);
                if (result.hit) {
                    world->SetBlock(result.blockPos.x, result.blockPos.y, result.blockPos.z, 0);
                }
                leftMouseDown = true;
            }
        } else {
            leftMouseDown = false;
        }

        // Right Click: Place Block
        if(glfwGetMouseButton(m_Window->GetGLFWwindow(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
        {
            if (!rightMouseDown) {
                e::RaycastResult result = world->Raycast(camera.position, camera.orientation, 10.0f);
                if (result.hit) {
                    // Place block at the position relative to the hit face normal
                    glm::ivec3 placePos = result.blockPos + result.normal;
                    world->SetBlock(placePos.x, placePos.y, placePos.z, currentPlacingBlockId); // 1 is our basic block type
                }
                rightMouseDown = true;
            }
        } else {
            rightMouseDown = false;
        }
    }
    
    void LoadTextures()
    {


        // 1. Identify common textures we want to use
        std::vector<std::string> textureFiles = {
            (e::Utils::GetRootDir() / "textures/blocks/grass_top.png").string(),
            (e::Utils::GetRootDir() / "textures/blocks/dirt.png").string(),
            (e::Utils::GetRootDir() / "textures/blocks/stone_generic.png").string(),
            (e::Utils::GetRootDir() / "textures/blocks/dirt.png").string(),
            (e::Utils::GetRootDir() / "textures/blocks/oak_log_side.png").string(),
            (e::Utils::GetRootDir() / "textures/blocks/cobblestone.png").string(),
            (e::Utils::GetRootDir() / "textures/blocks/glass.png").string(),
            (e::Utils::GetRootDir() / "textures/blocks/sandstone.png").string()
        };

        // 2. Initialize TextureArray with the number of textures we have
        // Assuming 16x16 pixels based on voxel standards and file sizes
        texArray = std::make_unique<e::TextureArray>(16, 16, (uint32_t)textureFiles.size());

        // 3. Load each texture into its respective layer
        for (uint32_t i = 0; i < textureFiles.size(); ++i) {
            texArray->AddTexture(textureFiles[i], i);
        }
    }
    
public:
    Sandbox() : camera({ 8.0f, 50.0f, 40.0f }, m_Window.get())
    {
        DebugWindowInit();   
        LoadShaders();
        LoadTextures();
        SetupOutlineBuffer();

        if (!world->LoadFromFile("world.dat")) {
            world->GenerateWorld(5); // Start small, let Update load more
        }
    }

    ~Sandbox()
    {
        world->SaveToFile("world.dat");
        world->UnloadAllChunks();
        DebugWindowShutdown();
    }

    void OnUpdate() override
    {
        glEnable(GL_DEPTH_TEST);
        e::Renderer::SetClearColor({ skyColor, 1.0f });
        e::Renderer::Clear();

        // Dynamically load/generate chunks around camera
        world->Update(camera.position, renderDistance);

        if (objShader) {
            objShader->Bind();
            
            glm::mat4 viewProj = camera.GetViewProjectionMatrix(45.0f, 0.1f, 1000.0f);
            objShader->SetUniformMat4("u_ViewProj", viewProj);

            objShader->SetUniformFloat3("lightPos", lightPos);
            objShader->SetUniformFloat3("viewPos", camera.position);
            objShader->SetUniformFloat3("lightColor", { 1.0f, 1.0f, 1.0f });
            objShader->SetUniformFloat3("objectColor", objectColor);

            if (texArray) {
                texArray->Bind(0);
                objShader->SetUniformInt("u_Textures", 0);
            }

            world->Draw(objShader, camera.position, camera.orientation, renderDistance);
            DrawOutline();
        }
        
        camera.Inputs();
        Input();
        DebugWindowRender();
    }
};

int main()
{
    Sandbox* app = new Sandbox();
    app->Run();
    delete app;
    return 0;
}
