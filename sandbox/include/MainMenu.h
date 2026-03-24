#ifndef MAINMENU_H
#define MAINMENU_H

#include <Scene.h>
#include <Window.h>
#include <ShapeUI.h>
#include <Event.h>
#include <Utils.h>
#include <Renderer.h>

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

    void LoadShaders();
    void DrawUI();
    void Input();

public:
    MainMenu(std::shared_ptr<e::Window> window, e::Event* event);
    void Update() override;
};

#endif
