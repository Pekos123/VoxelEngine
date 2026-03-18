#include <UIBlockDisplay.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

namespace e {
    UIBlockDisplay::UIBlockDisplay() {
        // --- Inside UIBlockDisplay Constructor ---
        float vertices[] = {
            // Pos (x, y)   UV (u, v)
            0.0f, 0.0f,     0.0f, 0.0f, // 0: Top-Left
            0.0f, 1.0f,     0.0f, 1.0f, // 1: Bottom-Left
            1.0f, 1.0f,     1.0f, 1.0f, // 2: Bottom-Right
            1.0f, 0.0f,     1.0f, 0.0f  // 3: Top-Right
        };

        // CCW Winding:
        // Tri 1: 0 -> 1 -> 2 (TL -> BL -> BR)
        // Tri 2: 2 -> 3 -> 0 (BR -> TR -> TL)
        unsigned indices[] = { 
            0, 1, 2, 
            2, 3, 0 
        };

        vao = std::make_shared<e::VertexArray>();
        vao->Bind();
        vbo = std::make_shared<e::VertexBuffer>(vertices, sizeof(vertices));
        vbo->SetLayout({
            { e::ShaderDataType::Float2, "aPos" },
            { e::ShaderDataType::Float2, "aTexCoord" }
        });
        ibo = std::make_shared<e::IndexBuffer>(indices, sizeof(indices) / sizeof(unsigned));
        vao->AddVertexBuffer(vbo);
        vao->SetIndexBuffer(ibo);
    }

    void UIBlockDisplay::CompileShaders(const std::string& vertexSrc, const std::string& fragmentSrc) {
        m_Shader = std::make_shared<Shader>(vertexSrc, fragmentSrc);
    }

    void UIBlockDisplay::DrawBlockIcon(uint32_t textureID, int blockId, glm::vec2 screenPos, float size, int screenWidth, int screenHeight) {
        if (!m_Shader) return;

        // --- Bulletproof State Management ---
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE); // <--- IMPORTANT: Prevents the square from vanishing if winding is wrong
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        m_Shader->Bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, textureID); // Use ARRAY target
        m_Shader->SetUniformInt("u_Texture", 0);
        m_Shader->SetUniformInt("u_Layer", blockId - 1); // Adjust ID to index

        // 2. Orthographic Projection (0,0 is Top-Left)
        glm::mat4 proj = glm::ortho(0.0f, (float)screenWidth, (float)screenHeight, 0.0f, -1.0f, 1.0f);

        // 3. Model Transform (Translate to screen pixels, then scale to size)
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(screenPos, 0.0f));
        model = glm::scale(model, glm::vec3(size, size, 1.0f));

        m_Shader->SetUniformMat4("u_Proj", proj);
        m_Shader->SetUniformMat4("u_Model", model);

        // 4. Draw Call
        Renderer::DrawIndexed(vao);
        // --- Reset State ---
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glDisable(GL_BLEND);
    }
}