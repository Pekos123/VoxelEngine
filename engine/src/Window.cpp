#include <Window.h>
#include <stdexcept>

namespace e
{
    Window::Window(unsigned int width, unsigned int height, const char* title)
        : width(width), height(height), title(title)
    {
        if (!glfwInit())
        {
            throw std::runtime_error("Failed to initialize GLFW");
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        
        window = glfwCreateWindow(width, height, title, nullptr, nullptr);
        if (!window)
        {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }

        glfwMakeContextCurrent(window);
        glfwSwapInterval(1); // Enable VSync

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            throw std::runtime_error("Failed to initialize GLAD");
        }

        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, Window::framebuffer_size_callback);

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
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void Window::PollEvents()
    {
        glfwPollEvents();
        glfwGetWindowSize(window, &width, &height);
    }

    void Window::SwapBuffers()
    {
        // This function should be implemented to swap buffers of the window
        glfwSwapBuffers(window);
    }
    
    bool Window::ShouldClose() const
    {
        // This function should be implemented to check if the window should close
        return glfwWindowShouldClose(window);
    }
}