#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <string>
#include <vector>
#include <memory>

namespace e
{
    class TextureArray
    {
    public:
        TextureArray(uint32_t width, uint32_t height, uint32_t layers);
        ~TextureArray();

        void AddTexture(const std::string& path, uint32_t layer);
        void Bind(uint32_t slot = 0) const;

        uint32_t GetID() const { return m_RendererID; }

    private:
        uint32_t m_RendererID;
        uint32_t m_Width, m_Height, m_Layers;
    };
}

#endif // TEXTURE_H
