#include <Utils.h>
#include <iostream>

namespace e
{
    namespace Utils
    {
        std::string ReadFile(const std::string& path)
        {
            std::ifstream file(path);
            if (!file)
                std::cerr << "Failed to open file: " << path;
        
            std::ostringstream ss;
            ss << file.rdbuf();
            return ss.str();
        }

        uint32_t packVertex(uint32_t x, uint32_t y, uint32_t z, uint32_t ao, uint32_t face, uint32_t vIdx)
        {
            return (x & 0x1F) |          // bits 0-4 (X: 0-31)
                   ((y & 0xFF) << 5) |   // bits 5-12 (Y: 0-255)
                   ((z & 0x1F) << 13) |  // bits 13-17 (Z: 0-31)
                   ((ao & 0x3) << 18) |  // bits 18-19 (AO: 0-3)
                   ((face & 0x7) << 20) |// bits 20-22 (Face: 0-7)
                   ((vIdx & 0x3) << 23); // bits 23-24 (vIdx: 0-3)
        }

        void addPackedFace(std::vector<uint32_t>& vertices, int x, int y, int z, FaceDirection dir, const float* ao)
        {
            uint32_t face;
            switch (dir) {
                case FaceDirection::TOP:    face = 0; break;
                case FaceDirection::BOTTOM: face = 1; break;
                case FaceDirection::FRONT:  face = 2; break;
                case FaceDirection::BACK:   face = 3; break;
                case FaceDirection::LEFT:   face = 4; break;
                case FaceDirection::RIGHT:  face = 5; break;
                default: face = 0; break;
            }

            // Pack 4 corners (0-3) and create 2 triangles (0,1,2, 0,2,3)
            uint32_t pAO[4];
            for (int i = 0; i < 4; i++) pAO[i] = (uint32_t)(ao[i] * 3.0f 
            + 0.5f); // Convert to 0,1,2 based on AO level, adding 0.5f for proper rounding bc of 0.999f exceptions

            uint32_t v0 = packVertex(x, y, z, pAO[0], face, 0);
            uint32_t v1 = packVertex(x, y, z, pAO[1], face, 1);
            uint32_t v2 = packVertex(x, y, z, pAO[2], face, 2);
            uint32_t v3 = packVertex(x, y, z, pAO[3], face, 3);

            vertices.push_back(v0);
            vertices.push_back(v1);
            vertices.push_back(v2);
            vertices.push_back(v0);
            vertices.push_back(v2);
            vertices.push_back(v3);
        }
    }
}
