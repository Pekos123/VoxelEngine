#include "Sandbox.h"
#include <filesystem>

int main()
{    
    Sandbox* app = new Sandbox("minecraft clone");
    app->Run();
    delete app;
    return 0;
}
