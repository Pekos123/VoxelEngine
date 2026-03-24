#ifndef SHADOWMAP_H
#define SHADOWMAP_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace e
{
    class ShadowMap
    {
    public:
        ShadowMap(uint32_t size);
        ~ShadowMap();

        void Bind() const;
        void Unbind() const;

        void BindTexture(uint32_t slot = 0) const;

        uint32_t GetTextureID() const { return m_DepthTexture; }
        uint32_t GetSize() const { return m_Size; }

        glm::mat4 GetLightSpaceMatrix(const glm::vec3& sunPos, const glm::vec3& playerPos);

    private:
        uint32_t m_Fbo;
        uint32_t m_DepthTexture;
        uint32_t m_Size;
    };
}

#endif // SHADOWMAP_H
