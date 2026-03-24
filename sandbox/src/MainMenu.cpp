#include "MainMenu.h"
#include <iostream>

MainMenu::MainMenu(std::shared_ptr<e::Window> window, e::Event* event) 
    : squere(squereSize), window(window), changeSceneEvent(event), text()
{
    LoadShaders();
    squere.pos = {100, 100};

    std::string fontPath = (e::Utils::GetRootDir() / "engine/fonts/PixelifySans.ttf").string();
    text.LoadFont(fontPath, 24);
}

void MainMenu::Update()
{
    e::Renderer::SetClearColor({backgroundColor, 1.0f});
    e::Renderer::Clear();

    DrawUI();
    Input();
}

void MainMenu::LoadShaders()
{
    std::string uiVPath = (e::Utils::GetRootDir() / "engine/shaders/ui/ui.vert").string();
    std::string uiFPath = (e::Utils::GetRootDir() / "engine/shaders/ui/ui.frag").string();
    std::string uiFSrc = e::Utils::ReadFile(uiFPath);
    std::string uiVSrc = e::Utils::ReadFile(uiVPath);

    std::string textVPath = (e::Utils::GetRootDir() / "engine/shaders/text/text.vert").string();
    std::string textFPath = (e::Utils::GetRootDir() / "engine/shaders/text/text.frag").string();
    std::string textFSrc = e::Utils::ReadFile(textFPath);
    std::string textVSrc = e::Utils::ReadFile(textVPath);

    squere.CompileShaders(uiVSrc, uiFSrc);
    text.CompileShaders(textVSrc, textFSrc);
}

void MainMenu::DrawUI()
{
    int width = window->GetWidth();
    int height = window->GetHeight();

    squere.pos = {width/2 - (squere.size.x / 2), height/2 - (squere.size.y / 2)};
    squere.Draw(width, height);

    text.pos = {width/2 - (text.fontSize * (int)(text.text.length() / 2)), height/2 - (text.fontSize/2)};
    text.text = "PLAY";
    text.Draw(width, height);
}

void MainMenu::Input()
{
    if(glfwGetMouseButton(window->GetGLFWwindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        double mx, my;
        glfwGetCursorPos(window->GetGLFWwindow(), &mx, &my);

        bool isInWidth = mx >= squere.pos.x && mx <= squere.pos.x + squere.size.x;
        bool isInHeight = my >= squere.pos.y && my <= squere.pos.y + squere.size.y;
        if(isInWidth && isInHeight) changeSceneEvent->Invoke();
    }
}
