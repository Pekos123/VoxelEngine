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

        uint32_t GetTextureID() const { return depthTexture; }
        uint32_t GetSize() const { return size; }

        glm::mat4 GetLightSpaceMatrix(const glm::vec3& sunPos, const glm::vec3& playerPos);

    private:
        uint32_t fbo;
        uint32_t depthTexture;
        uint32_t size;
    };
}

#endif // SHADOWMAP_H
