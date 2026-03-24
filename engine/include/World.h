#ifndef WORLD_H
#define WORLD_H

#include <Utils.h>
#include <Renderer.h>
#include <Shader.h>
#include <VertexArray.h>
#include <Buffer.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/hash.hpp>

#include <map>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_set>


#include <OpenSimplex/OpenSimplex2S.hpp>

// dodaj hight bedacy 128/256 czy cus
#define CHUNK_SIZE 16
#define CHUNK_HEIGHT 128

namespace e
{
    class Shader;
    class World;

    enum BlocksID
    {
        AIR,
        GRASS,
        DIRT,
        STONE,
        OAK_PLANKS,
        OAK_LOG,
        COBBLESTONE,
        GLASS,
        OAK_LEAVES,
        SANDSTONE // LAST BLOCK ALWAYS
    };

    class TerrainGenerator
    {
    public:
        OpenSimplex2S noiseGen;
        TerrainGenerator(int seed) : noiseGen(seed)
        {
        }
        int getHeight(int x, int z)
        {
            // 1. Continental Noise (Very slow changes, defines biomes/mountains)
            double continentFreq = 0.003; 
            double mask = (noiseGen.noise2(x * continentFreq, z * continentFreq) + 1.0) * 0.5;
            
            // Push the mask to extremes so mountains are localized but big
            // High power (e.g. 3.0) makes mountains rarer and steeper
            mask = std::pow(mask, 3.0);

            // 2. Detail Noise (The actual hills/mountains shape)
            double detailFreq = 0.015;
            double detail = (noiseGen.noise2(x * detailFreq, z * detailFreq) + 1.0) * 0.5;

            // 3. Combine
            float baseHeight = 32.0f;       // Minimum land height
            float mountainScale = 90.0f;  // How big mountains can get
            
            return static_cast<int>(baseHeight + (detail * mountainScale * mask));
        }
    };

    // each chunk will have their vbo and vao just for now
    struct Chunk
    {
        uint8_t blocks[CHUNK_SIZE][CHUNK_HEIGHT][CHUNK_SIZE]; // width, height, depth
        glm::ivec3 position; // chunk position in world coordinates

        bool dirty = false;
        bool tressGenerated = false;
        bool isGenerating = false;

        std::shared_ptr<VertexArray> vao;
        std::shared_ptr<VertexBuffer> vbo;
        int vertexCount = 0;
        
        void GenerateData(TerrainGenerator& gen, World* world); // step 1: fill array
        void GenerateTerrain(TerrainGenerator& gen);
        void GenerateTrees(TerrainGenerator& gen, World* world);
        void GenerateMesh(World* world); // step 2: create vertices
        
        void DeleteBuffers() {
            vao.reset();
            vbo.reset();
        }
    };

    struct RaycastResult {
        bool hit;
        glm::ivec3 blockPos;
        glm::ivec3 normal;
    };

    class World
    {
    public:
        void GenerateWorld(int worldSizeInChunks);
        void Update(const glm::vec3& cameraPos, float renderDistance);
        void Draw(const std::shared_ptr<Shader>& shader, const glm::vec3& cameraPos, const glm::vec3& cameraDir, float renderDistance);
        void DrawShadows(const std::shared_ptr<Shader>& shadowShader, const glm::vec3& cameraPos, float renderDistance);

        void SaveToFile(const std::string& filename);
        bool LoadFromFile(const std::string& filename);

        void LoadChunk(const glm::ivec3& chunkPos);
        void UnloadChunk(const glm::ivec3& chunkPos);
        void UnloadAllChunks();
        Chunk* GetChunk(const glm::ivec3& chunkPos);
        void MarkChunkAffected(int x, int y, int z, std::unordered_set<Chunk*>& set);

        void AddTree(int x, int y, int z);

        //Debug
        int GetLoadedChunkCount() const { return (int)m_Chunks.size(); }

        // Helper to get block at any world coordinate
        uint8_t GetBlock(int x, int y, int z);
        void SetBlock(int x, int y, int z, uint8_t type);
        // Setting block without rebuild all neighnour etc.
        bool SetBlockData(int x, int y, int z, uint8_t type);
        void RebuildChunkMesh(int chunkX, int chunkZ);

        RaycastResult Raycast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance);

        World(int seed, const std::string& savePath) : m_Gen(seed), m_SavePath(savePath) {}

    private:
        void SaveChunkToDisk(const Chunk& chunk);
        void UnloadChunks(const glm::vec3& cameraPos, float renderDistance);

        TerrainGenerator m_Gen;
        std::unordered_map<glm::ivec3, Chunk> m_Chunks;
        std::string m_SavePath;
        float m_TimeSinceLastUnload = 0.0f;
    };
}

#endif // WORLD_H