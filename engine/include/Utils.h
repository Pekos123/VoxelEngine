#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <filesystem>

#include <glm/glm.hpp>

namespace e
{
    enum class FaceDirection {
        FRONT, BACK, LEFT, RIGHT, TOP, BOTTOM
    };

    namespace Utils
    {
        std::string ReadFile(const std::string& filepath);
        std::filesystem::path GetRootDir();
        
        uint32_t packVertex(uint32_t x, uint32_t y, uint32_t z, uint32_t ao, uint32_t face, uint32_t vIdx, uint32_t blockID);
        void addPackedFace(std::vector<uint32_t>& vertices, int x, int y, int z, FaceDirection dir, const float* ao, uint32_t blockID);
    }
}

#endif // UTILS_H