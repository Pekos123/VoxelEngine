#include <Camera.h>

namespace e
{
    Camera::Camera(int width, int height, glm::vec3 pos, e::Window* window)
        : width(width), height(height), position(pos), window(window)
    {
        glfwSetInputMode(window->GetGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    void Camera::Matrix(float FOV, float near, float far, e::Shader& shader, const char* uniform)
    {
        glm::mat4 view = glm::lookAt(position, position + orientation, up);
        // Fix: Use float division for aspect ratio
        glm::mat4 proj = glm::perspective(glm::radians(FOV), (float)width / (float)height, near, far);
        glm::mat4 mvp = proj * view;

        shader.Bind();
        shader.SetUniformMat4(uniform, mvp);
    }

    glm::mat4 Camera::GetViewProjectionMatrix(float FOV, float near, float far) const
    {
        glm::mat4 view = glm::lookAt(position, position + orientation, up);
        glm::mat4 proj = glm::perspective(glm::radians(FOV), (float)width / (float)height, near, far);
        return proj * view;
    }

    // should be uopdated every frame with current window and shader

    void Camera::MouseMovement()
    {
        double mouseX, mouseY;
        glfwGetCursorPos(window->GetGLFWwindow(), &mouseX, &mouseY);

        // Calculate offset from center
        float rotX = sensivity * (float)(mouseY - (height / 2)) / height;
        float rotY = sensivity * (float)(mouseX - (width / 2)) / width;
        // Apply rotation to orientation
        // Note: We use raw values here because sensivity handles the scaling
        glm::vec3 newOrientation = glm::rotate(orientation, glm::radians(-rotX * 100.0f), glm::normalize(glm::cross(orientation, up)));
        
        // Limit pitch to prevent flipping
        if(abs(glm::angle(newOrientation, up) - glm::radians(90.0f)) <= glm::radians(85.0f))
            orientation = newOrientation;
            
        orientation = glm::rotate(orientation, glm::radians(-rotY * 100.0f), up);
        // Re-center cursor
        glfwSetCursorPos(window->GetGLFWwindow(), (double)width / 2, (double)height / 2);
    }

    void Camera::Inputs()
    {
        // Movement keys
        if(glfwGetKey(window->GetGLFWwindow(), GLFW_KEY_W) == GLFW_PRESS)
            position += speed * orientation * Renderer::deltaTime;
        if(glfwGetKey(window->GetGLFWwindow(), GLFW_KEY_S) == GLFW_PRESS)
            position += speed * -orientation * Renderer::deltaTime;
        if(glfwGetKey(window->GetGLFWwindow(), GLFW_KEY_A) == GLFW_PRESS)
            position += speed * -glm::normalize(glm::cross(orientation, up)) * Renderer::deltaTime;
        if(glfwGetKey(window->GetGLFWwindow(), GLFW_KEY_D) == GLFW_PRESS)
            position += speed * glm::normalize(glm::cross(orientation, up)) * Renderer::deltaTime;
        if (glfwGetKey(window->GetGLFWwindow(), GLFW_KEY_SPACE) == GLFW_PRESS)
		    position += speed * up * Renderer::deltaTime;
	    if (glfwGetKey(window->GetGLFWwindow(), GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		    position += speed * -up * Renderer::deltaTime;

        // Speed boost
        if(glfwGetKey(window->GetGLFWwindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            speed = 120.0f;
        else
            speed = 40.0f;

        // Mouse movement
        Camera::MouseMovement();
    }
}

