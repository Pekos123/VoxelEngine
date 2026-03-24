#ifndef WINDOW_H
#define WINDOW_H
#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>


namespace e
{
    class Window
    {
    private:
        GLFWwindow* m_GLFWWindow;
        int m_Width, m_Height;
        const char* m_Title;

        static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    public:
        Window(unsigned int width, unsigned int height, const char* title);
        ~Window();

        void PollEvents();
        void SwapBuffers();
        bool ShouldClose() const;

        int GetWidth() const { return m_Width; }
        int GetHeight() const { return m_Height; }

        GLFWwindow* GetGLFWwindow() const { return m_GLFWWindow; }
    };
}
#endif // WINDOW_H