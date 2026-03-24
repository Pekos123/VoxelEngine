#include "Renderer.h"

namespace e
{
    float Renderer::m_LastTime = 0.0f;
    float Renderer::deltaTime = 0.0f;

    void Renderer::Clear()
    {
        deltaTime = (float)glfwGetTime() - m_LastTime;
        m_LastTime = (float)glfwGetTime();
            
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Renderer::SetClearColor(const glm::vec4& color)
    {
        glClearColor(color.r, color.g, color.b, color.a);
    }

    void Renderer::Draw(const std::shared_ptr<VertexArray>& vertexArray, uint32_t vertexCount)
    {
        vertexArray->Bind();
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    }

    void Renderer::DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray)
    {
        vertexArray->Bind();
        vertexArray->GetIndexBuffer()->Bind();
        glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, 0);
    }
}