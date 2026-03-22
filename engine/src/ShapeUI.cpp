#include <ShapeUI.h>
#include <iostream>

using namespace e::UI;

Squere::Squere(glm::ivec2 size) : size(size)
{
    const std::string deafultPath = (e::Utils::GetRootDir() / "engine/textures/defaultUI.png").string();
    txt = new Texture2D(deafultPath);

    BindBuffers();
}
void Squere::SetTexture(const std::string& path)
{   
    txt = new Texture2D(path);
}
void Squere::CompileShaders(const std::string& vertexSrc, const std::string& fragmentSrc) {
    m_Shader = std::make_shared<Shader>(vertexSrc, fragmentSrc);
}
void Squere::BindBuffers()
{
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
void Squere::Draw(int screenWidth, int screenHeight)
{
    if(!m_Shader)
    {
        std::cout << "NO SHADER APPLIED TO e::UI::Shape\n";
        return;
    }
    // --- Bulletproof State Management ---
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE); // <--- IMPORTANT: Prevents the square from vanishing if winding is wrong
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // shader binding
    m_Shader->Bind();
    // texture binding
    txt->Bind();
    // setting up uniforms
    m_Shader->SetUniformInt("u_Texture", 0);
    // ortographic
    glm::mat4 proj = glm::ortho(0.0f, (float)screenWidth, (float)screenHeight, 0.0f, -1.0f, 1.0f);
    // translate to screen pixels 
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(pos, 0.0f));
    model = glm::scale(model, glm::vec3(size.x, size.y, 1.0f));

    m_Shader->SetUniformMat4("u_Proj", proj);
    m_Shader->SetUniformMat4("u_Model", model);

    // Draw Call
    Renderer::DrawIndexed(vao);

    // --- Reset State ---
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);
}