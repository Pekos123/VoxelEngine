#include <Texture.h>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace e
{
    TextureArray::TextureArray(uint32_t width, uint32_t height, uint32_t layers)
        : m_Width(width), m_Height(height), m_Layers(layers)
    {
        glGenTextures(1, &m_RendererID);
        glBindTexture(GL_TEXTURE_2D_ARRAY, m_RendererID);

        // Allocate storage
        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, width, height, layers, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    TextureArray::~TextureArray()
    {
        glDeleteTextures(1, &m_RendererID);
    }

    void TextureArray::AddTexture(const std::string& path, uint32_t layer)
    {
        if (layer >= m_Layers) {
            std::cerr << "Texture layer index " << layer << " out of bounds (" << m_Layers << ")" << std::endl;
            return;
        }

        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 4);

        if (data)
        {
            if (width != m_Width || height != m_Height)
            {
                std::cerr << "Texture " << path << " dimensions (" << width << "x" << height << ") do not match TextureArray (" << m_Width << "x" << m_Height << ")" << std::endl;
                stbi_image_free(data);
                return;
            }

            glBindTexture(GL_TEXTURE_2D_ARRAY, m_RendererID);
            glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, layer, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
            
            stbi_image_free(data);
        }
        else
        {
            std::cerr << "Failed to load texture: " << path << std::endl;
        }
    }

    void TextureArray::Bind(uint32_t slot) const
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D_ARRAY, m_RendererID);
    }
}
