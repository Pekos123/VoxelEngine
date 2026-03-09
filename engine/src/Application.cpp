#include "Application.h"
#include <glad/glad.h>

namespace e
{
    Application::Application()
    {
        m_Window = std::make_unique<Window>(1280, 720, "Game Engine");
    }

    Application::~Application()
    {
    }

    void Application::Run()
    {
        while (m_Running)
        {
            if (m_Window->ShouldClose())
                m_Running = false;

            m_Window->PollEvents();
            
            OnUpdate();
            
            m_Window->SwapBuffers();
        }
    }
}