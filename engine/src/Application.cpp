#include "Application.h"


namespace e
{
    Application::Application(const std::string& title)
    {
        m_Window = std::make_shared<Window>(1280, 720, title.c_str());
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