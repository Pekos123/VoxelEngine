#ifndef SANDBOX_H
#define SANDBOX_H

#include <Application.h>
#include <Scene.h>
#include <Event.h>
#include <memory>
#include <string>

class Sandbox : public e::Application
{
private:
    std::unique_ptr<e::Scene> currScene;
    e::Event e;
    std::string savePath;

    void ChangeSceneToGame();

public:
    Sandbox(const std::string& windowTitle, const std::string& savePath);
    void OnUpdate() override;
};

#endif
