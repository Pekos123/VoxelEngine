#ifndef RENDERER_H
#define RENDERER_H

#pragma once
#include "VertexArray.h"
#include "Shader.h"

#include <glfw/glfw3.h>

#include <map>

namespace e
{
    class Renderer
    {
    private:
        static float m_LastTime;
    public:
        static void Clear();
        static void SetClearColor(const glm::vec4& color);
        
        static void Draw(const std::shared_ptr<VertexArray>& vertexArray, uint32_t vertexCount);
        static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray);
    
        static float deltaTime; // Time between current frame and last frame
    };
}
#endif // RENDERER_H