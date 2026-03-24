#pragma once
#include "Window.h"

#include <glad/glad.h>

#include <memory>
#include <string>

namespace e
{
    class Application
    {
    public:
        Application(const std::string& title);
        virtual ~Application();

        virtual void OnUpdate() {}
        
        void Run();
        
        bool ShouldClose() const { return m_Window->ShouldClose(); }
        void OnWindowUpdate() { m_Window->PollEvents(); m_Window->SwapBuffers(); }

    protected:
        std::shared_ptr<Window> m_Window;
        bool m_Running = true;
    };

    // To be defined in CLIENT
    Application* CreateApplication();
}