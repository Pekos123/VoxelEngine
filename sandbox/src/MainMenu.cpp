#include "MainMenu.h"
#include <iostream>
#include <fstream>
#include <filesystem>

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
    ChooseSaveDebug();
}

void MainMenu::ChooseSaveDebug()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    {
        ImGui::Text("List of saves");
        std::vector<std::string> names = GetSavesNames();
        for(std::string name : names)
        {
            if(ImGui::Button(name.c_str(), {120, 30}))
                LoadSave(name);
        }
        if(ImGui::Button("CreateSave")) showSaveCreatingWindow = !showSaveCreatingWindow;
    }

    if(showSaveCreatingWindow) CreateNewSaveWindow();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
void MainMenu::CreateNewSaveWindow()
{
    ImGui::Begin("Create save");

    ImGui::InputText("Name of save: ", saveName, 25);
    ImGui::InputInt("Seed: ", &Game::seed);
    ImGui::Spacing();
    if(ImGui::Button("Create", {120, 30}))
        LoadSave(saveName);

    ImGui::End();
}
void MainMenu::LoadSave(const std::string& name)
{
    Sandbox::choosedSave = name;
    std::string fullPath = "saves/" + name;
    std::string metadataPath = fullPath + "/world.metadata";

    // if world path dont exist create it
    if (!std::filesystem::exists(fullPath)) std::filesystem::create_directories(fullPath);

    // if .metadata file exist load seed from it, if not create it with default seed
    if (std::filesystem::exists(metadataPath)) 
    {
        std::ifstream in(metadataPath);
        if (in.is_open()) in >> Game::seed;
    }
    // else create it and write to it 
    else 
    {
        std::ofstream out(metadataPath);
        if (out.is_open()) out << Game::seed;
    }

    changeSceneEvent->Invoke();
}
std::vector<std::string> MainMenu::GetSavesNames()
{
    std::vector<std::string> names;
    std::string savesDir = "saves";
    for (const auto& entry : std::filesystem::directory_iterator(savesDir)) {
        if (entry.is_directory()) {
            names.push_back(entry.path().filename().string());
        }
    }
    return names;
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
