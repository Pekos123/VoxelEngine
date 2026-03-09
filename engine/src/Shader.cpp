#include "Shader.h"
#include <glad/glad.h>
#include <iostream>
#include <vector>
#include <glm/gtc/type_ptr.hpp>

namespace e
{
    Shader::Shader(const std::string& vertexSrc, const std::string& fragmentSrc)
    {
        uint32_t vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSrc);
        uint32_t fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSrc);

        m_RendererID = glCreateProgram();
        glAttachShader(m_RendererID, vertexShader);
        glAttachShader(m_RendererID, fragmentShader);
        glLinkProgram(m_RendererID);

        int isLinked = 0;
        glGetProgramiv(m_RendererID, GL_LINK_STATUS, &isLinked);
        if (isLinked == GL_FALSE)
        {
            int maxLength = 0;
            glGetProgramiv(m_RendererID, GL_INFO_LOG_LENGTH, &maxLength);
            std::vector<char> infoLog(maxLength);
            glGetProgramInfoLog(m_RendererID, maxLength, &maxLength, &infoLog[0]);
            glDeleteProgram(m_RendererID);
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            std::cout << "Shader Link Error: " << infoLog.data() << std::endl;
            return;
        }

        glDetachShader(m_RendererID, vertexShader);
        glDetachShader(m_RendererID, fragmentShader);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    Shader::~Shader()
    {
        glDeleteProgram(m_RendererID);
    }

    void Shader::Bind() const
    {
        glUseProgram(m_RendererID);
    }

    void Shader::Unbind() const
    {
        glUseProgram(0);
    }

    void Shader::SetUniformFloat3(const std::string& name, const glm::vec3& value)
    {
        int location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform3f(location, value.x, value.y, value.z);
    }

    void Shader::SetUniformMat4(const std::string& name, const glm::mat4& value)
    {
        int location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }
    void Shader::SetUniformFloat(const std::string& name, float value)
    {
        int location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform1f(location, value);
    }

    uint32_t Shader::CompileShader(uint32_t type, const std::string& source)
    {
        if (source.empty()) {
            std::cerr << "CRITICAL: Shader source for " << (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment") << " is empty!" << std::endl;
            return 0;
        }

        uint32_t id = glCreateShader(type);
        const char* src = source.c_str();
        glShaderSource(id, 1, &src, nullptr);
        glCompileShader(id);

        int isCompiled = 0;
        glGetShaderiv(id, GL_COMPILE_STATUS, &isCompiled);
        if (isCompiled == GL_FALSE)
        {
            int maxLength = 0;
            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);
            std::vector<char> infoLog(maxLength);
            glGetShaderInfoLog(id, maxLength, &maxLength, &infoLog[0]);
            glDeleteShader(id);
            std::cout << "Shader Compile Error (" << (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment") << "): " << infoLog.data() << std::endl;
            return 0;
        }
        return id;
    }
}