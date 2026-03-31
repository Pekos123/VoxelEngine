#ifndef MAINMENU_H
#define MAINMENU_H

#include <Scene.h>
#include <Window.h>
#include <ShapeUI.h>
#include <Event.h>
#include <Utils.h>
#include <Renderer.h>
#include <Sandbox.h>
#include <Game.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <memory>
#include <string>
#include <glm/glm.hpp>

class MainMenu : public e::Scene
{
private:
    e::Event* changeSceneEvent;
    std::shared_ptr<e::Window> window;

    glm::vec3 backgroundColor = {.8f, .8f, .8f};
    glm::ivec2 squereSize = {380, 120};
    e::UI::Rectangle squere;
    e::UI::Text text;

    bool showSaveCreatingWindow = false;
    char saveName[25] = "";

    void LoadShaders();
    void DrawUI();
    void Input();

    void ChooseSaveDebug();
    void CreateNewSaveWindow();
    void LoadSave(const std::string& name);
    std::vector<std::string> GetSavesNames();

public:
    MainMenu(std::shared_ptr<e::Window> window, e::Event* event);

    void Update() override;
};

#endif
