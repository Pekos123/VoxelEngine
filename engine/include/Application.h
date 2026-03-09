#pragma once
#include "Window.h"
#include <memory>

namespace e
{
    class Application
    {
    public:
        Application();
        virtual ~Application();

        virtual void OnUpdate() {}
        
        void Run();
        
        bool ShouldClose() const { return m_Window->ShouldClose(); }
        void OnWindowUpdate() { m_Window->PollEvents(); m_Window->SwapBuffers(); }

    protected:
        std::unique_ptr<Window> m_Window;
        bool m_Running = true;
    };

    // To be defined in CLIENT
    Application* CreateApplication();
}