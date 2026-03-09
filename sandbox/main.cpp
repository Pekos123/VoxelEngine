#include <Application.h>
#include <Renderer.h>
#include <VertexArray.h>
#include <Shader.h>
#include <Camera.h>
#include <World.h>
#include <Utils.h>

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

    glm::vec3 lightPos = { 10.0f, 20.0f, 10.0f };
    glm::vec3 objectColor = { 0.4f, 0.8f, 0.3f }; // Nice green
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
                ImGui::DragFloat3("Light Position", glm::value_ptr(lightPos), 0.5f);
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

    void LoadShaders()
    {
        std::filesystem::path currentDir = std::filesystem::current_path();
        std::filesystem::path rootDir;
        
        std::filesystem::path checkDir = currentDir;
        for (int i = 0; i < 5; ++i) {
            if (std::filesystem::exists(checkDir / "build")) {
                rootDir = checkDir;
                break;
            }
            if (checkDir.has_parent_path()) {
                checkDir = checkDir.parent_path();
            } else {
                break;
            }
        }

        if (rootDir.empty()) {
            std::cerr << "CRITICAL: Could not find project root containing 'build' folder!" << std::endl;
            return;
        }

        std::string vPath = (rootDir / "engine/shaders/obj/obj.vert").string();
        std::string fPath = (rootDir / "engine/shaders/obj/obj.frag").string();

        std::string vSrc = e::Utils::ReadFile(vPath);
        std::string fSrc = e::Utils::ReadFile(fPath);

        if (vSrc.empty() || fSrc.empty()) {
            std::cerr << "CRITICAL: Shader source is empty!" << std::endl;
            return;
        }

        objShader = std::make_shared<e::Shader>(vSrc, fSrc);
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
                    world->SetBlock(placePos.x, placePos.y, placePos.z, 1); // 1 is our basic block type
                }
                rightMouseDown = true;
            }
        } else {
            rightMouseDown = false;
        }
    }
    
public:
    Sandbox() : camera({ 8.0f, 20.0f, 40.0f }, m_Window.get())
    {
        DebugWindowInit();   
        LoadShaders();

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
        e::Renderer::SetClearColor({ 0.1f, 0.1f, 0.15f, 1.0f });
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

            world->Draw(objShader, camera.position, renderDistance);
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
