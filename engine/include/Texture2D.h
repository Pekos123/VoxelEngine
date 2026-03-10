#ifndef TEXTURE2D_H
#define TEXTURE2D_H

#include <string>

namespace e {
    class Texture2D {
    public:
        Texture2D(const std::string& path);
        ~Texture2D();

        void Bind(uint32_t slot = 0) const;
        uint32_t GetID() const { return m_RendererID; }

    private:
        uint32_t m_RendererID;
        int m_Width, m_Height, m_Channels;
    };
}

#endif // TEXTURE2D_H
