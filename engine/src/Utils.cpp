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

        std::filesystem::path GetRootDir()
        {
            std::filesystem::path currentDir = std::filesystem::current_path();
            std::filesystem::path rootDir;
            
            std::filesystem::path checkDir = currentDir;
            for (int i = 0; i < 5; ++i) {
                if (std::filesystem::exists(checkDir / "build")) {
                    rootDir = checkDir;
                    break;
                }
                if (checkDir.has_parent_path()) {
                    checkDir = checkDir.parent_path();
                } else {
                    break;
                }
            }

            if (rootDir.empty()) {
                std::cerr << "CRITICAL: Could not find project root containing 'build' folder! Returning current folder" << std::endl;
                return currentDir;
            }
            return rootDir;
        }
        uint32_t packVertex(uint32_t x, uint32_t y, uint32_t z, uint32_t ao, uint32_t face, uint32_t vIdx, uint32_t blockID)
        {
            return (x & 0x1F) |          // bits 0-4 (X: 0-31)
                   ((y & 0xFF) << 5) |   // bits 5-12 (Y: 0-255)
                   ((z & 0x1F) << 13) |  // bits 13-17 (Z: 0-31)
                   ((ao & 0x3) << 18) |  // bits 18-19 (AO: 0-3)
                   ((face & 0x7) << 20) |// bits 20-22 (Face: 0-7)
                   ((vIdx & 0x3) << 23) |// bits 23-24 (vIdx: 0-3)
                   ((blockID & 0x7F) << 25); // bits 25-31 (BlockID: 0-127)
        }

        void addPackedFace(std::vector<uint32_t>& vertices, int x, int y, int z, FaceDirection dir, const float* ao, uint32_t blockID)
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

            uint32_t v0 = packVertex(x, y, z, pAO[0], face, 0, blockID);
            uint32_t v1 = packVertex(x, y, z, pAO[1], face, 1, blockID);
            uint32_t v2 = packVertex(x, y, z, pAO[2], face, 2, blockID);
            uint32_t v3 = packVertex(x, y, z, pAO[3], face, 3, blockID);

            vertices.push_back(v0);
            vertices.push_back(v1);
            vertices.push_back(v2);
            vertices.push_back(v0);
            vertices.push_back(v2);
            vertices.push_back(v3);
        }
        int GetRandomNum(int min, int max){return rand()%(max-min + 1) + min;}
    }
}
