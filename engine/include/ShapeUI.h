#ifndef SHAPEUI_H
#define SHAPEUI_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Buffer.h>
#include <VertexArray.h>
#include <Shader.h>
#include <Texture2D.h>
#include <Renderer.h>

#include <vector>
#include <memory>
#include <iostream>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace e
{
    namespace UI
    {
        class Shape /// do interhance later right now just make simple squere work
        {   
        private:
            std::vector<float> vertices;
            std::vector<unsigned> indecies;
        public:
            glm::ivec2 size;
            glm::ivec2 pos;
        
            void Draw();
        };
        class Squere : Shape
        {
        private:
            Texture2D* txt;
        
            std::shared_ptr<e::VertexArray> vao;
            std::shared_ptr<e::VertexBuffer> vbo;
            std::shared_ptr<e::IndexBuffer> ibo;
            
            std::shared_ptr<Shader> m_Shader;
            
            void BindBuffers();
        public:
            Squere(glm::ivec2 size);

            glm::ivec2 size;
            glm::ivec2 pos;
        
            void SetTexture(const std::string& path);
            void Draw(int screenWidth, int screenHeight);
            void CompileShaders(const std::string& vertexSrc, const std::string& fragmentSrc);
        };
        struct Character {
            unsigned int TextureID;  // ID handle of the glyph texture
            glm::ivec2   Size;       // Size of glyph
            glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
            unsigned int Advance;    // Offset to advance to next glyph
        };
        class Text
        {
        private:
            e::Shader* shader;
            std::shared_ptr<e::VertexArray> vao;
            std::shared_ptr<e::VertexBuffer> vbo;
            
            std::map<char, Character> Characters;
        public:
            glm::vec3 color = {.0f, .0f, .0f};
            glm::ivec2 pos = {100, 100};
            float scale = 1.0f;
            int fontSize;
            std::string text;
        
            Text();
            void Draw(int screenWidth, int screenHeight);
            void CompileShaders(const std::string& vertexSrc, const std::string& fragmentSrc);
            void LoadFont(const std::string& fontPath, unsigned int fontSize);
        };
    }
}

#endif