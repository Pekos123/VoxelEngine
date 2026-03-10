#ifndef UIBLOCKDISPLAY_H
#define UIBLOCKDISPLAY_H

#include <Shader.h>
#include <glm/glm.hpp>
#include <memory>
#include <string>

namespace e {
    class UIBlockDisplay {
    public:
        UIBlockDisplay();
        ~UIBlockDisplay();

        // Compiles shaders from source strings
        void CompileShaders(const std::string& vertexSrc, const std::string& fragmentSrc);
        
        // Renders the icon. For now, it will output solid green to verify geometry.
        void DrawBlockIcon(uint32_t textureID, int blockId, glm::vec2 screenPos, float size, int screenWidth, int screenHeight);

    private:
        uint32_t m_VaoID = 0;
        uint32_t m_VboID = 0;
        uint32_t m_IboID = 0;
        
        std::shared_ptr<Shader> m_Shader;
        
        const float ATLAS_SLOTS = 16.0f; 
    };
}

#endif // UIBLOCKDISPLAY_H
