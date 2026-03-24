#include "Sandbox.h"
#include "MainMenu.h"
#include "Game.h"

Sandbox::Sandbox(const std::string& windowTitle, const std::string& savePath) 
    : Application(windowTitle), savePath(savePath)
{
    currScene = std::make_unique<MainMenu>(m_Window, &e);
    e.AddListener(this, &Sandbox::ChangeSceneToGame);
}

void Sandbox::ChangeSceneToGame()
{
    currScene = std::make_unique<Game>(savePath, m_Window);
}

void Sandbox::OnUpdate()
{
    currScene->Update();
}
