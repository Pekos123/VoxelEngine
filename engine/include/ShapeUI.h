#ifndef SHAPEUI_H
#define SHAPEUI_H

#include <Buffer.h>
#include <VertexArray.h>
#include <Shader.h>
#include <Texture2D.h>
#include <Renderer.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <memory>

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
    }
}

#endif