#include "Sandbox.h"
#include <filesystem>

int main()
{
    std::string savePath = "saves/world";
    if (!std::filesystem::exists("saves")) {
        std::filesystem::create_directory("saves");
    }
    if (!std::filesystem::exists(savePath)) {
        std::filesystem::create_directory(savePath);
    }

    Sandbox* app = new Sandbox("minecraft clone", savePath);
    app->Run();
    delete app;
    return 0;
}
