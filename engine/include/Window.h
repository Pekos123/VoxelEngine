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
        GLFWwindow* window;
        int width, height;
        const char* title;

        static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    public:
        Window(unsigned int width, unsigned int height, const char* title);
        ~Window();

        void PollEvents();
        void SwapBuffers();
        bool ShouldClose() const;

        int GetWidth() const { return width; }
        int GetHeight() const { return height; }

        GLFWwindow* GetGLFWwindow() const { return window; }
    };
}
#endif // WINDOW_H