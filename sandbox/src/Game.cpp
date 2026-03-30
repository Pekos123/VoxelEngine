#include "Game.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

constexpr glm::vec3 PLAYER_START_POS = { 8.0f, 80.0f, 40.0f };
constexpr glm::vec2 SQUERE_SIZE = { 80, 80 };
constexpr int SEED = 55555;
constexpr int SHADOW_MAP_SIZE = 2048;
Game::Game(const std::string& savePath, std::shared_ptr<e::Window> window) 
    : player(PLAYER_START_POS), camera(PLAYER_START_POS, window), squere(SQUERE_SIZE), window(window)
{
    world = new e::World(SEED, savePath); // seed and save path
    shadowMap = std::make_unique<e::ShadowMap>(SHADOW_MAP_SIZE);
    
    DebugWindowInit();   
    LoadTextures();
    LoadShaders();
    SetupOutlineBuffer();

    squere.SetTexture(textureFiles[currentPlacingBlockId-1]);

    glfwSetWindowUserPointer(window->GetGLFWwindow(), this);
    glfwSetScrollCallback(window->GetGLFWwindow(), scroll_callback);

    world->GenerateWorld(5); 
}

Game::~Game()
{
    world->UnloadAllChunks();
    delete world;
    DebugWindowShutdown();
}

void Game::Update()
{
    e::Renderer::SetClearColor({ skyColor, 1.0f });
    e::Renderer::Clear();

    world->Update(camera.position, renderDistance);
    DrawWorld();
    DrawUI();

    camera.Inputs();
    Input();
    PlayerMovement();

    DebugWindowRender(); 
}

void Game::MainDebugWindow()
{
    ImGui::Begin("Debug Window");
    {
        ImGui::Text("Chunks: %d", 0); 
        ImGui::ColorEdit3("Object Color", glm::value_ptr(objectColor));
        ImGui::ColorEdit3("Sky Color", glm::value_ptr(skyColor));
        ImGui::DragFloat3("Light Position", glm::value_ptr(lightPos), 0.5f);
        ImGui::ColorEdit3("Outline Color", glm::value_ptr(outlineColor));
        ImGui::DragFloat("Outline Thickness", &outlineThickness, 0.1f, 0.1f, 5.0f);
        ImGui::DragInt("Block Id", &currentPlacingBlockId, 1, 0, e::BlockID::COUNT);
        ImGui::DragFloat3("SunPos: ", glm::value_ptr(sunPos), 0.05f, -1.0f, 1.f);
        if(ImGui::Button("Recompile shaders", {150, 30}))
            LoadShaders();
    }
    ImGui::End();
}
void Game::PlayerDebugWindow()
{
    ImGui::Begin("Player");
    {
        ImGui::InputFloat("Sensivity", &camera.sensitivity, 0.1f, 0.5f);
        ImGui::InputFloat("Camera Speed", &camSpeed, 0.1f);
        ImGui::SliderFloat("Render Distance", &renderDistance, 32.0f, 800.0f);
        ImGui::DragFloat("Fov", &fov, 0.1f, 20.f, 120.f);
        ImGui::DragFloat("Movemnt Speed (def: 5.0)", &player.speed, 1.0f, 1.0f, 100.f);
        ImGui::DragFloat("Jump force (def: 8.0)", &player.jumpForce, 1.0f, 1.0f, 100.f);
        ImGui::DragFloat("Gravity (def: -25.0)", &player.gravity, 1.0f, -100.f, 0.f);
        ImGui::Checkbox("Free fly", &freeCam);
    }    
    ImGui::End();
}
void Game::PerformanceDebugWindow()
{
    ImGui::Begin("Performance");
    {
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        
        e::RaycastResult result = world->Raycast(camera.position, camera.orientation, 10.0f);
        if (result.hit) {
            ImGui::Text("Target Block: %d, %d, %d", result.blockPos.x, result.blockPos.y, result.blockPos.z);
        } else {
            ImGui::Text("Target Block: None");
        }
        ImGui::Text("Renderer chunks: %d", world->GetLoadedChunkCount());
    }
    ImGui::End();
}
void Game::DebugWindowRender()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    RenderCrosshair();

    // Debug's menus
    {
        MainDebugWindow();
        PlayerDebugWindow();
        PerformanceDebugWindow();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Game::DebugWindowInit()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    
    ImGui_ImplGlfw_InitForOpenGL(window->GetGLFWwindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void Game::DebugWindowShutdown()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Game::RenderCrosshair()
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 center = viewport->GetCenter();
    ImDrawList* draw_list = ImGui::GetForegroundDrawList();

    float crosshairSize = 10.0f;
    float thickness = 2.0f;
    ImU32 color = IM_COL32(255, 255, 255, 200);

    draw_list->AddLine(
        ImVec2(center.x - crosshairSize, center.y),
        ImVec2(center.x + crosshairSize, center.y),
        color, thickness
    );
    draw_list->AddLine(
        ImVec2(center.x, center.y - crosshairSize),
        ImVec2(center.x, center.y + crosshairSize),
        color, thickness
    );
}

void Game::SetupOutlineBuffer()
{
    float cubeVertices[] = {
        0,0,0,  1,0,0,  1,1,0,  0,1,0,
        0,0,1,  1,0,1,  1,1,1,  0,1,1
    };
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

void Game::LoadShaders()
{
    std::string vPath = (e::Utils::GetRootDir() / "engine/shaders/obj/obj.vert").string();
    std::string fPath = (e::Utils::GetRootDir() / "engine/shaders/obj/obj.frag").string();
    std::string vSrc = e::Utils::ReadFile(vPath);
    std::string fSrc = e::Utils::ReadFile(fPath);

    std::string outlineVPath = (e::Utils::GetRootDir() / "engine/shaders/outline/outline.vert").string();
    std::string outlineFPath = (e::Utils::GetRootDir() / "engine/shaders/outline/outline.frag").string();
    std::string outlineFSrc = e::Utils::ReadFile(outlineFPath);
    std::string outlineVSrc = e::Utils::ReadFile(outlineVPath);

    std::string uiVPath = (e::Utils::GetRootDir() / "engine/shaders/ui/ui.vert").string();
    std::string uiFPath = (e::Utils::GetRootDir() / "engine/shaders/ui/ui.frag").string();
    std::string uiFSrc = e::Utils::ReadFile(uiFPath);
    std::string uiVSrc = e::Utils::ReadFile(uiVPath);

    std::string shadowVPath = (e::Utils::GetRootDir() / "engine/shaders/shadow/shadow.vert").string();
    std::string shadowFPath = (e::Utils::GetRootDir() / "engine/shaders/shadow/shadow.frag").string();
    std::string shadowVSrc = e::Utils::ReadFile(shadowVPath);
    std::string shadowFSrc = e::Utils::ReadFile(shadowFPath);

    if (vSrc.empty() || fSrc.empty()) {
        std::cerr << "CRITICAL: Obj (blocks) shader source is empty!" << std::endl;
        return;
    }
    if (outlineVSrc.empty() || outlineFSrc.empty()) {
        std::cerr << "CRITICAL: Outline shader source is empty!" << std::endl;
        return;
    }
    if (uiVSrc.empty() || uiFSrc.empty()) {
        std::cerr << "CRITICAL: UI shader source is empty!" << std::endl;
        return;
    }
    if (shadowFSrc.empty() || shadowVSrc.empty()) {
        std::cerr << "CRITICAL: Shadow shader source is empty!" << std::endl;
        return;
    }

    objShader = std::make_shared<e::Shader>(vSrc, fSrc);
    outlineShader = std::make_shared<e::Shader>(outlineVSrc, outlineFSrc);
    shadowShader = std::make_shared<e::Shader>(shadowVSrc, shadowFSrc);
    squere.CompileShaders(uiVSrc, uiFSrc);
}

void Game::LoadTextures() 
{
    textureFiles = {
        (e::Utils::GetRootDir() / "textures/blocks/grass_top.png").string(),
        (e::Utils::GetRootDir() / "textures/blocks/dirt.png").string(),
        (e::Utils::GetRootDir() / "textures/blocks/stone_generic.png").string(),
        (e::Utils::GetRootDir() / "textures/blocks/oak_planks.png").string(),
        (e::Utils::GetRootDir() / "textures/blocks/oak_log_side.png").string(),
        (e::Utils::GetRootDir() / "textures/blocks/cobblestone.png").string(),
        (e::Utils::GetRootDir() / "textures/blocks/glass.png").string(),
        (e::Utils::GetRootDir() / "textures/blocks/oak_leaves.png").string(),
        (e::Utils::GetRootDir() / "textures/blocks/water.png").string(),
        (e::Utils::GetRootDir() / "textures/blocks/sandstone.png").string(),
        (e::Utils::GetRootDir() / "textures/blocks/sand_ugly_2.png").string()
    };

    texArray = std::make_unique<e::TextureArray>(16, 16, (uint32_t)textureFiles.size());
    for (uint32_t i = 0; i < textureFiles.size(); ++i) {
        texArray->AddTexture(textureFiles[i], i);
    }
}

void Game::DrawOutline(glm::mat4 viewProj)
{
    e::RaycastResult result = world->Raycast(camera.position, camera.orientation, 10.0f);
    if(result.hit)
    {
        glDepthMask(GL_FALSE); 
        glDepthFunc(GL_LEQUAL); 
        glLineWidth(outlineThickness);

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
        glLineWidth(1.0f);
    }
}

void Game::DrawUI()
{
    int width = window->GetWidth();
    int height = window->GetHeight();

    squere.pos = {width / 2 - (squere.size.x/2), height - 150}; 
    squere.Draw(width, height);
}

void Game::DrawWorld()
{
    if (objShader && shadowShader) {
        glm::mat4 lightSpaceMatrix = shadowMap->GetLightSpaceMatrix(sunPos, camera.position);
        shadowShader->Bind();
        shadowShader->SetUniformMat4("u_LightSpaceMatrix", lightSpaceMatrix);
        
        shadowMap->Bind();
        world->DrawShadows(shadowShader, camera.position, renderDistance);
        shadowMap->Unbind();

        glViewport(0, 0, window->GetWidth(), window->GetHeight());

        objShader->Bind();
        
        glm::mat4 viewProj = camera.GetViewProjectionMatrix(fov, 0.1f, 1000.0f);
        objShader->SetUniformMat4("u_View", camera.GetViewMatrix(fov, 0.1f, 1000.f));
        objShader->SetUniformMat4("u_ViewProj", viewProj);
        objShader->SetUniformMat4("u_LightSpaceMatrix", lightSpaceMatrix);

        float fogEnd = renderDistance * 0.8;
        float fogStart = renderDistance * 0.5f;
        objShader->SetUniformFloat("fogStart", fogStart);
        objShader->SetUniformFloat("fogEnd", fogEnd);
        objShader->SetUniformFloat3("fogColor", {0.8f, 0.8f, 0.8f});

        objShader->SetUniformFloat3("viewPos", camera.position);
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

constexpr float DEFAULT_SPEED = 40.0f;
constexpr float SPRINT_SPEED = 120.0f;
void Game::Input()
{
    if (ImGui::GetIO().WantCaptureMouse) return;

    // Named constants for camera movement speeds
    if(glfwGetKey(window->GetGLFWwindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camSpeed = SPRINT_SPEED;
    else
        camSpeed = DEFAULT_SPEED;

    // Destroying block
    if(glfwGetMouseButton(window->GetGLFWwindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        if (!leftMouseDown) {
            e::RaycastResult result = world->Raycast(camera.position, camera.orientation, 10.0f);
            if (result.hit) {
                int blockId = world->GetBlock(result.blockPos.x, result.blockPos.y, result.blockPos.z);
                e::BlockData data = e::BlockDatabase::Get(static_cast<e::BlockID>(blockId));
                
                if(data.type != e::BlockType::LIQUID) world->SetBlock(result.blockPos.x, result.blockPos.y, result.blockPos.z, e::BlockID::AIR);
            }
            leftMouseDown = true;
        }
    } else {
        leftMouseDown = false;
    }
    // Placing block
    if(glfwGetMouseButton(window->GetGLFWwindow(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        if (!rightMouseDown) {
            e::RaycastResult result = world->Raycast(camera.position, camera.orientation, 10.0f);
            if (result.hit) {
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
void Game::PlayerMovement()
{
    if(freeCam) 
    {
        player.position = camera.position - glm::vec3(0.0f, camHeight, 0.0f);
        CameraMovement();
    }
    else 
    {
        player.Update(e::Renderer::deltaTime, *world);
        camera.position = player.position + glm::vec3(0.0f, camHeight, 0.0f);
        player.HandleInput(window->GetGLFWwindow(), camera.orientation);
    }
}
void Game::CameraMovement()
{
    // Directional vectors
    glm::vec3 right = glm::normalize(glm::cross(camera.orientation, camera.up));

    if(glfwGetKey(window->GetGLFWwindow(), GLFW_KEY_W) == GLFW_PRESS)
        camera.position += camSpeed * camera.orientation * e::Renderer::deltaTime;
    if(glfwGetKey(window->GetGLFWwindow(), GLFW_KEY_S) == GLFW_PRESS)
        camera.position += camSpeed * -camera.orientation * e::Renderer::deltaTime;
    if(glfwGetKey(window->GetGLFWwindow(), GLFW_KEY_A) == GLFW_PRESS)
        camera.position += camSpeed * -right * e::Renderer::deltaTime;
    if(glfwGetKey(window->GetGLFWwindow(), GLFW_KEY_D) == GLFW_PRESS)
        camera.position += camSpeed * right * e::Renderer::deltaTime;
    if (glfwGetKey(window->GetGLFWwindow(), GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.position += camSpeed * camera.up * e::Renderer::deltaTime;
    if (glfwGetKey(window->GetGLFWwindow(), GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera.position += camSpeed * -camera.up * e::Renderer::deltaTime;
}

void Game::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    Game* instance = static_cast<Game*>(glfwGetWindowUserPointer(window));
    if (instance) 
    {
        instance->currentPlacingBlockId += (int)yoffset;
        int firstBlock = e::BlockID::GRASS;
        int lastBlock = e::BlockID::COUNT-1;

        if (instance->currentPlacingBlockId > lastBlock) instance->currentPlacingBlockId = firstBlock;
        if (instance->currentPlacingBlockId < firstBlock) instance->currentPlacingBlockId = lastBlock;
        
        instance->squere.SetTexture(instance->textureFiles[instance->currentPlacingBlockId-1]);
    }
}
