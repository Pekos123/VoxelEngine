#include <UIBlockDisplay.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

namespace e {
    UIBlockDisplay::UIBlockDisplay() {
        // --- Bulletproof VAO/VBO Setup ---
        // 4 vertices: x, y (pos), u, v (tex)
        float vertices[] = {
            // Pos (2f)   UV (2f)
            0.0f, 0.0f,   0.0f, 0.0f, // 0: TL
            1.0f, 0.0f,   1.0f, 0.0f, // 1: TR
            1.0f, 1.0f,   1.0f, 1.0f, // 2: BR
            0.0f, 1.0f,   0.0f, 1.0f  // 3: BL
        };
        uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };

        glGenVertexArrays(1, &m_VaoID);
        glGenBuffers(1, &m_VboID);
        glGenBuffers(1, &m_IboID);

        glBindVertexArray(m_VaoID);

        // Bind and fill VBO
        glBindBuffer(GL_ARRAY_BUFFER, m_VboID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // Bind and fill IBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IboID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // Location 0: Position (x, y)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

        // Location 1: TexCoords (u, v)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        // Unbind VAO (safe practice)
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    UIBlockDisplay::~UIBlockDisplay() {
        if(m_VaoID) glDeleteVertexArrays(1, &m_VaoID);
        if(m_VboID) glDeleteBuffers(1, &m_VboID);
        if(m_IboID) glDeleteBuffers(1, &m_IboID);
    }

    void UIBlockDisplay::CompileShaders(const std::string& vertexSrc, const std::string& fragmentSrc) {
        m_Shader = std::make_shared<Shader>(vertexSrc, fragmentSrc);
    }

    void UIBlockDisplay::DrawBlockIcon(uint32_t textureID, int blockId, glm::vec2 screenPos, float size, int screenWidth, int screenHeight) {
        if (!m_Shader) return;

        // --- Bulletproof State Management ---
        glDisable(GL_DEPTH_TEST);  // Ensure UI is on top
        glDisable(GL_CULL_FACE);   // Ensure we don't accidentally cull the quad
        glEnable(GL_BLEND);        // Just in case we add alpha later
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
        glBindVertexArray(m_VaoID);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // --- Reset State ---
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
    }
}
