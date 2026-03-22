#include "ShadowMap.h"
#include <iostream>

namespace e
{
    ShadowMap::ShadowMap(uint32_t size)
        : size(size)
    {
        glGenFramebuffers(1, &fbo);

        glGenTextures(1, &depthTexture);
        glBindTexture(GL_TEXTURE_2D, depthTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, size, size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        
        // Critical: GL_CLAMP_TO_BORDER to prevent repeated shadows
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // Areas outside light box are unshadowed
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
        
        // No color buffer needed for shadow mapping
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cerr << "Shadow Map Framebuffer is incomplete!" << std::endl;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    ShadowMap::~ShadowMap()
    {
        glDeleteFramebuffers(1, &fbo);
        glDeleteTextures(1, &depthTexture);
    }

    void ShadowMap::Bind() const
    {
        glViewport(0, 0, size, size);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glClear(GL_DEPTH_BUFFER_BIT);
    }

    void ShadowMap::Unbind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void ShadowMap::BindTexture(uint32_t slot) const
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, depthTexture);
    }

    glm::mat4 ShadowMap::GetLightSpaceMatrix(const glm::vec3& sunPos, const glm::vec3& playerPos)
    {
        float range = 120.0f;
    
        // 1. Setup projection as usual
        glm::mat4 lightProjection = glm::ortho(-range, range, -range, range, 0.1f, 400.0f);
        
        // 2. Setup View
        glm::vec3 lightViewPos = playerPos + glm::normalize(sunPos) * 100.0f;
        glm::mat4 lightView = glm::lookAt(lightViewPos, playerPos, glm::vec3(0.0f, 1.0f, 0.0f));
        
        // 3. THE FIX: Create the combined matrix
        glm::mat4 lightSpaceMatrix = lightProjection * lightView;
    
        // 4. Snap the matrix to texel units
        // Project the origin (0,0,0) into light space
        glm::vec4 shadowOrigin = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        shadowOrigin = lightSpaceMatrix * shadowOrigin;
        
        // Convert to actual texel coordinates
        shadowOrigin *= ((float)size / 2.0f);
    
        // Calculate the rounding offset
        glm::vec4 roundedOrigin = glm::round(shadowOrigin);
        glm::vec4 roundOffset = roundedOrigin - shadowOrigin;
        
        // Convert back to light space units
        roundOffset *= (2.0f / (float)size);
        roundOffset.z = 0.0f;
        roundOffset.w = 0.0f;
    
        // Apply the offset to the projection matrix to "lock" the grid
        lightProjection[3] += roundOffset;
    
        return lightProjection * lightView;
    }
}
