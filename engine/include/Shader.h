#ifndef SHADER_H
#define SHADER_H
#pragma once

#include <string>
#include <Utils.h>
#include <glm/glm.hpp>

namespace e
{
    class Shader
    {
    public:
        Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
        ~Shader();

        void Bind() const;
        void Unbind() const;

        void SetUniformFloat3(const std::string& name, const glm::vec3& value);
        void SetUniformFloat2(const std::string& name, const glm::vec2& value);
        void SetUniformFloat4(const std::string& name, const glm::vec4& value);
        void SetUniformInt(const std::string& name, int value);
        void SetUniformMat4(const std::string& name, const glm::mat4& value);
        void SetUniformFloat(const std::string& name, float value);
        
        uint32_t GetRendererID() const { return m_RendererID; }

    private:
        uint32_t m_RendererID;
        uint32_t CompileShader(uint32_t type, const std::string& source);
    };
}
#endif // SHADER_H