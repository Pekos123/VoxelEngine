#include "Sandbox.h"
#include "MainMenu.h"
#include "Game.h"

Sandbox::Sandbox(const std::string& windowTitle) 
    : Application(windowTitle)
{
    DebugWindowInit();

    currScene = std::make_unique<MainMenu>(m_Window, &e);
    e.AddListener(this, &Sandbox::ChangeSceneToGame);

    // check if saves folder exist
    if (!std::filesystem::exists("saves")) {
        std::filesystem::create_directory("saves");
    }
}
void Sandbox::DebugWindowInit() // it has to innit in window class or app class
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    
    ImGui_ImplGlfw_InitForOpenGL(m_Window->GetGLFWwindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330");
}
void Sandbox::DebugWindowShutdown()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
void Sandbox::ChangeSceneToGame()
{
    currScene = std::make_unique<Game>(savePath + choosedSave, m_Window);
}

void Sandbox::OnUpdate()
{
    currScene->Update();
}
