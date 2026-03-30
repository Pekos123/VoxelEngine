#ifndef GAME_H
#define GAME_H

#include <Scene.h>
#include <Player.h>
#include <Camera.h>
#include <World.h>
#include <Window.h>
#include <ShapeUI.h>
#include <Shader.h>
#include <ShadowMap.h>
#include <Texture.h>
#include <Utils.h>
#include <Renderer.h>
#include <Block.h>

#include <memory>
#include <vector>
#include <string>
#include <glm/glm.hpp>

class Game : public e::Scene
{
private:
    e::Player player;
    e::Camera camera;
    e::World* world = nullptr;
    std::shared_ptr<e::Window> window;
    e::UI::Rectangle squere;
    
    std::shared_ptr<e::Shader> objShader;
    std::shared_ptr<e::Shader> outlineShader;
    std::shared_ptr<e::Shader> uiShader;
    std::shared_ptr<e::Shader> shadowShader;

    std::unique_ptr<e::ShadowMap> shadowMap;

    glm::vec3 lightPos = { 10.0f, 20.0f, 10.0f };
    glm::vec3 objectColor = { 1.f, 1.f, 1.0f };
    glm::vec3 skyColor = {0.7f, 0.7f, 0.95f};
    glm::vec3 sunPos = { 0.5f, 1.f, 0.3f };

    glm::vec3 outlineColor = { 0.0f, 0.0f, 0.0f };
    float outlineThickness = 2.0f;

    std::unique_ptr<e::VertexArray> outlineVAO;
    std::shared_ptr<e::VertexBuffer> vbo;
    std::shared_ptr<e::IndexBuffer> ib;

    std::unique_ptr<e::TextureArray> texArray;
    std::vector<std::string> textureFiles;
    int currentPlacingBlockId = e::BlockID::GRASS;
    
    bool freeCam = false;
    float camSpeed;
    float renderDistance = 120.0f;
    float fov = 75.0f;
    float camHeight = 1.7f;

    bool leftMouseDown = false;
    bool rightMouseDown = false;

    void MainDebugWindow();
    void PlayerDebugWindow();
    void PerformanceDebugWindow();
    void DebugWindowRender();
    void DebugWindowInit();
    void DebugWindowShutdown();
    void RenderCrosshair();
    
    void SetupOutlineBuffer();
    void LoadShaders();
    void LoadTextures();

    void DrawOutline(glm::mat4 viewProj);
    void DrawUI();
    void DrawWorld();
    
    void Input();
    void PlayerMovement();
    void CameraMovement();

    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

public:
    Game(const std::string& savePath, std::shared_ptr<e::Window> window);
    ~Game();

    void Update() override;
};

#endif
