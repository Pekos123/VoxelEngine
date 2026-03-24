#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/vector_angle.hpp"

#include <Shader.h>
#include <Window.h>
#include <Renderer.h>

namespace e
{
    class Camera
    {
        private:
            std::shared_ptr<e::Window> m_Window;

            float m_lastX, m_lastY;

            bool m_Foccused = true;
            bool m_FirstMouse = true;
        public:
            glm::vec3 position;
            glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f);
            glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

            float speed = 0.1f;
            float sensitivity = 1.0f;

            Camera(glm::vec3 pos, std::shared_ptr<e::Window> window);

            void Matrix(float FOV, float near, float far, e::Shader& shader, const char* uniform);
            void UpdateProjection(int currentWidth, int currentHeight);
            void Inputs();
            void MouseMovement();
            void CameraMovement();

            glm::mat4 GetViewProjectionMatrix(float FOV, float near, float far) const;
    };
}

#endif // CAMERA_H