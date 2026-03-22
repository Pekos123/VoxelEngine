#include <Application.h>
#include <Renderer.h>
#include <VertexArray.h>
#include <Shader.h>
#include <Camera.h>
#include <World.h>
#include <Utils.h>
#include <Texture.h>
#include <ShadowMap.h>
#include <Player.h>
#include <ShapeUI.h>

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
    e::Player player;
    e::Camera camera;
    e::World* world = nullptr;
    e::UI::Squere squere;
    // ... rest of class remains similar ...
    
    std::shared_ptr<e::Shader> objShader;
    std::shared_ptr<e::Shader> outlineShader;
    std::shared_ptr<e::Shader> uiShader;
    std::shared_ptr<e::Shader> shadowShader;

    std::unique_ptr<e::ShadowMap> shadowMap;

    glm::vec3 lightPos = { 10.0f, 20.0f, 10.0f };
    glm::vec3 objectColor = { 1.f, 1.f, 1.0f }; // White bc of textures
    glm::vec3 skyColor = {0.7f, 0.7f, 0.95f};
    glm::vec3 sunPos = { 0.5f, 1.f, 0.3f };

    // Outline
    glm::vec3 outlineColor = { 0.0f, 0.0f, 0.0f }; // Black
    float outlineThickness = 2.0f;

    // Buffers
    std::unique_ptr<e::VertexArray> outlineVAO;
    std::shared_ptr<e::VertexBuffer> vbo;
    std::shared_ptr<e::IndexBuffer> ib;

    // Texture
    std::unique_ptr<e::TextureArray> texArray;
    std::vector<std::string> textureFiles;
    int currentPlacingBlockId = e::BlocksID::GRASS;

    // Player
    bool freeCam = false;
    float renderDistance = 120.0f;
    float fov = 75.0f;

    // IMGUI 
    void DebugWindowRender()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        RenderCrosshair();

        {
            ImGui::Begin("Debug Window");
                ImGui::Text("Chunks: %d", 0); // Placeholder or add world.GetChunkCount()
                ImGui::ColorEdit3("Object Color", glm::value_ptr(objectColor));
                ImGui::ColorEdit3("Sky Color", glm::value_ptr(skyColor));
                ImGui::DragFloat3("Light Position", glm::value_ptr(lightPos), 0.5f);
                ImGui::ColorEdit3("Outline Color", glm::value_ptr(outlineColor));
                ImGui::DragFloat("Outline Thickness", &outlineThickness, 0.1f, 0.1f, 5.0f);
                ImGui::DragInt("Block Id", &currentPlacingBlockId, 1, e::BlocksID::GRASS, e::BlocksID::SANDSTONE);
                ImGui::DragFloat3("SunPos: ", glm::value_ptr(sunPos), 0.05f, -1.0f, 1.f);
                if(ImGui::Button("Recompile shaders", {150, 30}))
                    LoadShaders();
            ImGui::End();


            ImGui::Begin("Player");
                ImGui::InputFloat("Sensivity", &camera.sensitivity, 0.1f, 0.5f);
                ImGui::InputFloat("Camera Speed", &camera.speed, 0.1f);
                ImGui::SliderFloat("Render Distance", &renderDistance, 32.0f, 800.0f);
                ImGui::DragFloat("Fov", &fov, 0.1f, 20.f, 120.f);
                ImGui::DragFloat("Movemnt Speed (def: 5.0)", &player.speed, 1.0f, 1.0f, 100.f);
                ImGui::DragFloat("Jump force (def: 8.0)", &player.jumpForce, 1.0f, 1.0f, 100.f);
                ImGui::DragFloat("Gravity (def: -25.0)", &player.speed, 1.0f, -100.f, 0.f);
                ImGui::Checkbox("Free fly", &freeCam);
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
    // IMGUI END
   
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

        std::string uiVPath = (e::Utils::GetRootDir() / "engine/shaders/ui/ui.vert").string();
        std::string uiFPath = (e::Utils::GetRootDir() / "engine/shaders/ui/ui.frag").string();
        std::string uiFSrc = e::Utils::ReadFile(uiFPath);
        std::string uiVSrc = e::Utils::ReadFile(uiVPath); // Using same source for vert and frag

        std::string shadowVPath = (e::Utils::GetRootDir() / "engine/shaders/shadow/shadow.vert").string();
        std::string shadowFPath = (e::Utils::GetRootDir() / "engine/shaders/shadow/shadow.frag").string();
        std::string shadowVSrc = e::Utils::ReadFile(shadowVPath);
        std::string shadowFSrc = e::Utils::ReadFile(shadowFPath);

        if (vSrc.empty() || fSrc.empty()) {
            std::cerr << "CRITICAL: Shader source is empty!" << std::endl;
            return;
        }
        if (uiVSrc.empty() || uiFSrc.empty()) {
            std::cerr << "ui shader source empty! path: " << uiVPath << std::endl;
            return;
        }

        objShader = std::make_shared<e::Shader>(vSrc, fSrc);
        outlineShader = std::make_shared<e::Shader>(outlineVSrc, outlineFSrc); // Using same source for vert and frag
        shadowShader = std::make_shared<e::Shader>(shadowVSrc, shadowFSrc);
        squere.CompileShaders(uiVSrc, uiFSrc);
    }
    void LoadTextures() 
    {
        // 1. Identify common textures we want to use
        textureFiles = {
            (e::Utils::GetRootDir() / "textures/blocks/grass_top.png").string(),
            (e::Utils::GetRootDir() / "textures/blocks/dirt.png").string(),
            (e::Utils::GetRootDir() / "textures/blocks/stone_generic.png").string(),
            (e::Utils::GetRootDir() / "textures/blocks/oak_planks.png").string(),
            (e::Utils::GetRootDir() / "textures/blocks/oak_log_side.png").string(),
            (e::Utils::GetRootDir() / "textures/blocks/cobblestone.png").string(),
            (e::Utils::GetRootDir() / "textures/blocks/glass.png").string(),
            (e::Utils::GetRootDir() / "textures/blocks/oak_leaves.png").string(),
            // if u want to add new block place it here, also dont forget to update blocks in World.h -> BlocksID
            (e::Utils::GetRootDir() / "textures/blocks/sandstone.png").string() // lets sandstone be last block (HARDCODED)
        };

        // 2. Initialize TextureArray with the number of textures we have
        // Assuming 16x16 pixels based on voxel standards and file sizes
        texArray = std::make_unique<e::TextureArray>(16, 16, (uint32_t)textureFiles.size());

        // 3. Load each texture into its respective layer
        for (uint32_t i = 0; i < textureFiles.size(); ++i) {
            texArray->AddTexture(textureFiles[i], i);
        }
    }

    // draw outline around targeted block
    void DrawOutline(glm::mat4 viewProj)
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
            outlineShader->SetUniformMat4("u_ViewProj", viewProj);
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
    void DrawUI()
    {
        int width = m_Window->GetWidth();
        int height = m_Window->GetHeight();
    
        squere.pos = {width / 2 - (squere.size.x/2), height - 150}; 
        squere.Draw(width, height);
    }
    void DrawWorld()
    {
        if (objShader && shadowShader) {
            // 1. Shadow Pass
            glm::mat4 lightSpaceMatrix = shadowMap->GetLightSpaceMatrix(sunPos, camera.position);
            shadowShader->Bind();
            shadowShader->SetUniformMat4("u_LightSpaceMatrix", lightSpaceMatrix);
            
            shadowMap->Bind();
            world->DrawShadows(shadowShader, camera.position, renderDistance);
            shadowMap->Unbind();

            // Reset viewport to window size
            glViewport(0, 0, m_Window->GetWidth(), m_Window->GetHeight());

            // 2. Main Pass
            objShader->Bind();
            
            glm::mat4 viewProj = camera.GetViewProjectionMatrix(fov, 0.1f, 1000.0f);
            objShader->SetUniformMat4("u_ViewProj", viewProj);
            objShader->SetUniformMat4("u_LightSpaceMatrix", lightSpaceMatrix);

            objShader->SetUniformFloat3("lightPos", lightPos);
            objShader->SetUniformFloat3("sunPos", sunPos);
            objShader->SetUniformFloat3("viewPos", camera.position);
            objShader->SetUniformFloat3("lightColor", { 1.0f, 1.0f, 1.0f });
            objShader->SetUniformFloat3("objectColor", objectColor);

            if (texArray) {
                texArray->Bind(0);
                objShader->SetUniformInt("u_Textures", 0);
            }

            shadowMap->BindTexture(1);
            objShader->SetUniformInt("u_ShadowMap", 1);

            world->Draw(objShader, camera.position, camera.orientation, renderDistance);
            DrawOutline(viewProj);
        }
    }
    // INPUTS
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
                    world->SetBlock(result.blockPos.x, result.blockPos.y, result.blockPos.z, e::BlocksID::AIR);
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
                    
                    if (player.CanPlaceBlock(placePos)) {
                        world->SetBlock(placePos.x, placePos.y, placePos.z, currentPlacingBlockId);
                    }
                }
                rightMouseDown = true;
            }
        } else {
            rightMouseDown = false;
        }
    }
    void PlayerMovement()
    {
        // free cam iplementation
        if(freeCam) 
        {
            player.position = camera.position - glm::vec3(0.0f, 1.7f, 0.0f);
            camera.CameraMovement();
        }
        else 
        {
            // Update player physics
            player.Update(e::Renderer::deltaTime, *world);
            // Sync camera to player eye level
            camera.position = player.position + glm::vec3(0.0f, 1.7f, 0.0f);
            player.HandleInput(m_Window->GetGLFWwindow(), camera.orientation);
        }
    }
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
    {
        // 1. Get the pointer back from GLFW
        Sandbox* instance = static_cast<Sandbox*>(glfwGetWindowUserPointer(window));

        // 2. Safety check and logic
        if (instance) 
        {
            // Now you can access non-static variables!
            instance->currentPlacingBlockId += (int)yoffset;
            int firstBlock = e::BlocksID::GRASS;
            int lastBlock = e::BlocksID::SANDSTONE;

            // Wrap around logic
            if (instance->currentPlacingBlockId > lastBlock) instance->currentPlacingBlockId = firstBlock;
            if (instance->currentPlacingBlockId < firstBlock) instance->currentPlacingBlockId = lastBlock;
        }
        instance->squere.SetTexture(instance->textureFiles[instance->currentPlacingBlockId-1]);
    }   
    // INPUTS END
public:
    Sandbox(const std::string& savePath) 
        : player({ 8.0f, 80.0f, 40.0f }), camera({ 8.0f, 80.0f, 40.0f }, m_Window.get()), squere({80, 80})
    {
        world = new e::World(55555, savePath); // seed and save path
        shadowMap = std::make_unique<e::ShadowMap>(2048);
        
        DebugWindowInit();   
        LoadTextures();
        LoadShaders();
        SetupOutlineBuffer();

        squere.SetTexture(textureFiles[currentPlacingBlockId-1]);

        glfwSetWindowUserPointer(m_Window->GetGLFWwindow(), this);
        glfwSetScrollCallback(m_Window->GetGLFWwindow(), scroll_callback);

        // We don't need LoadFromFile anymore as LoadChunk handles it
        world->GenerateWorld(5); 
    }

    ~Sandbox()
    {
        world->UnloadAllChunks();
        delete world;
        DebugWindowShutdown();
    }

    void OnUpdate() override
    {
        // Update graphyics
        e::Renderer::SetClearColor({ skyColor, 1.0f });
        e::Renderer::Clear();

        // Dynamically load/generate chunks around camera
        world->Update(camera.position, renderDistance);
        DrawWorld();
        DrawUI();

        // i think i will need that to propely display ui
        // glm::mat4 uiProj = glm::ortho<glm::mat4>(0.0f, (float)m_Window->GetWidth(), (float)m_Window->GetWidth(), 0.0f, -1.0f, 1.0f);
        
        // Update inputs
        camera.Inputs();
        Input();
        PlayerMovement();

        DebugWindowRender(); // At the end to be on top of eveything else
    }
};

int main()
{
    std::string savePath = "saves/world";
    if (!std::filesystem::exists("saves")) {
        std::filesystem::create_directory("saves");
    }
    if (!std::filesystem::exists(savePath)) {
        std::filesystem::create_directory(savePath);
    }

    Sandbox* app = new Sandbox(savePath);
    app->Run();
    delete app;
    return 0;
}
