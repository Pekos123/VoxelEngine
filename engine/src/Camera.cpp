#include <Camera.h>

#define SENSIVITY_REGULATOR 1000

namespace e
{
    Camera::Camera(glm::vec3 pos, e::Window* window)
        : position(pos), window(window)
    {
        glfwSetInputMode(window->GetGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    void Camera::Matrix(float FOV, float near, float far, e::Shader& shader, const char* uniform)
    {
        glm::mat4 view = glm::lookAt(position, position + orientation, up);
        // Fix: Use float division for aspect ratio
        glm::mat4 proj = glm::perspective(glm::radians(FOV), (float)window->GetWidth() / (float)window->GetHeight(), near, far);
        glm::mat4 mvp = proj * view;

        shader.Bind();
        shader.SetUniformMat4(uniform, mvp);
    }

    glm::mat4 Camera::GetViewProjectionMatrix(float FOV, float near, float far) const
    {
        glm::mat4 view = glm::lookAt(position, position + orientation, up);
        glm::mat4 proj = glm::perspective(glm::radians(FOV), (float)window->GetWidth() / (float)window->GetHeight(), near, far);
        return proj * view;
    }

    void Camera::MouseMovement()
    {
        double mouseX, mouseY;
        glfwGetCursorPos(window->GetGLFWwindow(), &mouseX, &mouseY);

        // Initialize last positions on the very first frame
        if (firstMouse)
        {
            lastX = mouseX;
            lastY = mouseY;
            firstMouse = false;
        }

        // Calculate how much the mouse moved since the last frame
        float deltaX = (float)(mouseX - lastX);
        float deltaY = (float)(mouseY - lastY);

        // Update last positions for the next frame
        lastX = mouseX;
        lastY = mouseY;

        // Apply sensitivity to the raw movement
        float rotX = deltaY * (sensivity / SENSIVITY_REGULATOR);
        float rotY = deltaX * (sensivity / SENSIVITY_REGULATOR);

        // Rotate around the "Right" axis (cross product of orientation and up)
        glm::vec3 right = glm::normalize(glm::cross(orientation, up));
        glm::vec3 newOrientation = glm::rotate(orientation, glm::radians(-rotX * 100.0f), right);

        // Pitch limit (prevents flipping)
        if(abs(glm::angle(newOrientation, up) - glm::radians(90.0f)) <= glm::radians(88.0f))
            orientation = newOrientation;

        // Rotate around the World Up axis
        orientation = glm::rotate(orientation, glm::radians(-rotY * 100.0f), up);

        int width = window->GetWidth();
        int height = window->GetHeight();

        // Cursor MUST be locked in the middle
        glfwSetCursorPos(window->GetGLFWwindow(), (double)width / 2, (double)height / 2);
        
        lastX = (double)width / 2;
        lastY = (double)height / 2;
    }

    static bool foccusedAlreadyPressed = false;
    void Camera::Inputs()
    {
        if(glfwGetKey(window->GetGLFWwindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS && !foccusedAlreadyPressed)
        {

            foccusedAlreadyPressed = true;
            foccused = !foccused;
            if(foccused)
                glfwSetInputMode(window->GetGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            else
                glfwSetInputMode(window->GetGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        
        }
        else if(glfwGetKey(window->GetGLFWwindow(), GLFW_KEY_ESCAPE) == GLFW_RELEASE && foccusedAlreadyPressed) foccusedAlreadyPressed = false;
        
        if(!foccused) return; 

        // Speed boost
        if(glfwGetKey(window->GetGLFWwindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            speed = 120.0f;
        else
            speed = 40.0f;

        // Mouse movement
        Camera::MouseMovement();
    }

    void Camera::CameraMovement()
    {
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
    }
}

