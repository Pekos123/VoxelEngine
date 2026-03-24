#include <ShapeUI.h>

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
Text::Text()
{
    vao = std::make_shared<e::VertexArray>();
    vbo = std::make_shared<e::VertexBuffer>(nullptr, sizeof(float) * 6 * 4);
    vbo->SetLayout({
        { e::ShaderDataType::Float4, "aPosTex" }
    });
    vao->AddVertexBuffer(vbo);
}
void Text::CompileShaders(const std::string& vertexSrc, const std::string& fragmentSrc) 
{
    shader = new Shader(vertexSrc, fragmentSrc);
}
void Text::Draw(int screenWidth, int screenHeight)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    shader->Bind();
    shader->SetUniformFloat3("textColor", {color});

    glm::mat4 projection = glm::ortho(0.0f, (float)screenWidth, 0.0f, (float)screenHeight);
    shader->SetUniformMat4("projection", projection);

    glActiveTexture(GL_TEXTURE0);

    // iterate through all characters
    std::string::const_iterator c;
    int posx = pos.x; // we create duplicate of pos.x so it wont change orginal varible 
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        float xpos = posx + ch.Bearing.x * scale;
        float ypos = pos.y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
        
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }           
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        
        // update content of VBO memory
        vbo->SetData(vertices, sizeof(vertices));

        // render quad
        Renderer::Draw(vao, 6);
        
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        posx += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindTexture(GL_TEXTURE_2D, 0);   
}
void Text::LoadFont(const std::string& fontPath, unsigned int fontSize) {
    this->fontSize = fontSize;
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return;
    }

    FT_Face face;
    if (FT_New_Face(ft, fontPath.c_str(), 0, &face)) {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return;
    }

    FT_Set_Pixel_Sizes(face, 0, fontSize);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Wyłącz ograniczenie wyrównania bajtów

    for (unsigned char c = 0; c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) continue;

        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width,
                     face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE,
                     face->glyph->bitmap.buffer);
        
        // Ustawienia tekstury dla fontów
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = {
            texture, 
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            (unsigned int)face->glyph->advance.x
        };
        Characters.insert(std::pair<char, Character>(c, character));
    }
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}