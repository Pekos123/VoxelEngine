#include <Camera.h>

constexpr float MOUSE_ROTATION_SCALER = .1f; 
constexpr float MAX_NO_CLIP_ANGLE = 88.f;
constexpr float DEFAULT_SPEED = 40.0f;
constexpr float SPRINT_SPEED = 120.0f;

namespace e
{
    Camera::Camera(glm::vec3 pos, std::shared_ptr<e::Window> window)
        : position(pos), m_Window(window)
    {
        glfwSetInputMode(window->GetGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    void Camera::Matrix(float FOV, float near, float far, e::Shader& shader, const char* uniform)
    {
        glm::mat4 view = glm::lookAt(position, position + orientation, up);
        glm::mat4 proj = glm::perspective(glm::radians(FOV), (float)m_Window->GetWidth() / (float)m_Window->GetHeight(), near, far);
        glm::mat4 mvp = proj * view;

        shader.Bind();
        shader.SetUniformMat4(uniform, mvp);
    }

    glm::mat4 Camera::GetViewProjectionMatrix(float FOV, float near, float far) const
    {
        glm::mat4 view = glm::lookAt(position, position + orientation, up);
        glm::mat4 proj = glm::perspective(glm::radians(FOV), (float)m_Window->GetWidth() / (float)m_Window->GetHeight(), near, far);
        return proj * view;
    }
    glm::mat4 Camera::GetViewMatrix(float FOV, float near, float far)
    {
        return glm::perspective(glm::radians(FOV), (float)m_Window->GetWidth() / (float)m_Window->GetHeight(), near, far);
    }

    void Camera::MouseMovement()
    {
        double mouseX, mouseY;
        glfwGetCursorPos(m_Window->GetGLFWwindow(), &mouseX, &mouseY);

        // Initialize last positions on the very first frame
        if (m_FirstMouse)
        {
            m_lastX = mouseX;
            m_lastY = mouseY;
            m_FirstMouse = false;
        }

        // Calculate how much the mouse moved since the last frame
        float deltaX = (float)(mouseX - m_lastX);
        float deltaY = (float)(mouseY - m_lastY);

        // Update last positions for the next frame
        m_lastX = mouseX;
        m_lastY = mouseY;

        // Apply sensitivity 
        float rotX = deltaY * sensitivity;
        float rotY = deltaX * sensitivity;

        glm::vec3 right = glm::normalize(glm::cross(orientation, up));
        
        // Named variable replaces 100.0f
        float pitchAngle = glm::radians(-rotX * MOUSE_ROTATION_SCALER);
        glm::vec3 newOrientation = glm::rotate(orientation, pitchAngle, right);

        // Pitch limit (prevents flipping)
        float currentPitchFromUp = glm::angle(newOrientation, up);
        if(abs(currentPitchFromUp - glm::radians(90.0f)) <= glm::radians(MAX_NO_CLIP_ANGLE))
        {
            orientation = newOrientation;
        }

        // Named variable replaces 100.0f
        float yawAngle = glm::radians(-rotY * MOUSE_ROTATION_SCALER);
        orientation = glm::rotate(orientation, yawAngle, up);

        int width = m_Window->GetWidth();
        int height = m_Window->GetHeight();

        glfwSetCursorPos(m_Window->GetGLFWwindow(), (double)width / 2.0, (double)height / 2.0);
        
        m_lastX = (double)width / 2.0;
        m_lastY = (double)height / 2.0;
    }

    static bool foccusedAlreadyPressed = false;
    void Camera::Inputs()
    {
        if(glfwGetKey(m_Window->GetGLFWwindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS && !foccusedAlreadyPressed)
        {
            foccusedAlreadyPressed = true;
            m_Foccused = !m_Foccused;
            if(m_Foccused)
                glfwSetInputMode(m_Window->GetGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            else
                glfwSetInputMode(m_Window->GetGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        else if(glfwGetKey(m_Window->GetGLFWwindow(), GLFW_KEY_ESCAPE) == GLFW_RELEASE && foccusedAlreadyPressed) foccusedAlreadyPressed = false;
        
        if(!m_Foccused) return; 

        // Named constants for movement speeds
        if(glfwGetKey(m_Window->GetGLFWwindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            speed = SPRINT_SPEED;
        else
            speed = DEFAULT_SPEED;

        // MouseMovement
        Camera::MouseMovement();
    }

    void Camera::CameraMovement()
    {
        // Directional vectors
        glm::vec3 right = glm::normalize(glm::cross(orientation, up));

        if(glfwGetKey(m_Window->GetGLFWwindow(), GLFW_KEY_W) == GLFW_PRESS)
            position += speed * orientation * Renderer::deltaTime;
        if(glfwGetKey(m_Window->GetGLFWwindow(), GLFW_KEY_S) == GLFW_PRESS)
            position += speed * -orientation * Renderer::deltaTime;
        if(glfwGetKey(m_Window->GetGLFWwindow(), GLFW_KEY_A) == GLFW_PRESS)
            position += speed * -right * Renderer::deltaTime;
        if(glfwGetKey(m_Window->GetGLFWwindow(), GLFW_KEY_D) == GLFW_PRESS)
            position += speed * right * Renderer::deltaTime;
        if (glfwGetKey(m_Window->GetGLFWwindow(), GLFW_KEY_SPACE) == GLFW_PRESS)
            position += speed * up * Renderer::deltaTime;
        if (glfwGetKey(m_Window->GetGLFWwindow(), GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            position += speed * -up * Renderer::deltaTime;
    }
}