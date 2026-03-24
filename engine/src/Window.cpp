#include <Window.h>
#include <stdexcept>

namespace e
{
    Window::Window(unsigned int width, unsigned int height, const char* title)
        : m_Width(width), m_Height(height), m_Title(title)
    {
        if (!glfwInit())
        {
            throw std::runtime_error("Failed to initialize GLFW");
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        
        m_GLFWWindow = glfwCreateWindow(width, height, title, nullptr, nullptr);
        if (!m_GLFWWindow)
        {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }

        glfwMakeContextCurrent(m_GLFWWindow);
        glfwSwapInterval(1); // Enable VSync

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            throw std::runtime_error("Failed to initialize GLAD");
        }

        glfwSetWindowUserPointer(m_GLFWWindow, this);
        glfwSetFramebufferSizeCallback(m_GLFWWindow, Window::framebuffer_size_callback);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }

    void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height)
    { 
        // Tell OpenGL the new dimensions of the window
        glViewport(0, 0, width, height);
    }

    Window::~Window()
    {
        glfwDestroyWindow(m_GLFWWindow);
        glfwTerminate();
    }

    void Window::PollEvents()
    {
        glfwPollEvents();
        glfwGetWindowSize(m_GLFWWindow, &m_Width, &m_Height);
    }

    void Window::SwapBuffers()
    {
        // This function should be implemented to swap buffers of the window
        glfwSwapBuffers(m_GLFWWindow);
    }
    
    bool Window::ShouldClose() const
    {
        // This function should be implemented to check if the window should close
        return glfwWindowShouldClose(m_GLFWWindow);
    }
}