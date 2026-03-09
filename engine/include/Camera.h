#ifndef CAMERA_H
#define CAMERA_H

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
            e::Window* window;

        public:
            glm::vec3 position;
            glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f);
            glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

            float speed = 0.1f;
            float sensivity = 1.0f;

            Camera(glm::vec3 pos, e::Window* window);

            void Matrix(float FOV, float near, float far, e::Shader& shader, const char* uniform);
            void UpdateProjection(int currentWidth, int currentHeight);
            void Inputs();
            void MouseMovement();

            glm::mat4 GetViewProjectionMatrix(float FOV, float near, float far) const;
            glm::vec3 GetPosition() const { return position; }
    };
}

#endif // CAMERA_H