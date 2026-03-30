#include <World.h>

namespace e
{
    static float vertexAO(bool side1, bool side2, bool corner) {
        if (side1 && side2) return 0.0f;
        return (3.0f - (float(side1) + float(side2) + float(corner))) / 3.0f;
    }
    constexpr int waterLevel = 32; // global water level, can be changed later for more interesting terrain
    void Chunk::GenerateTerrain(TerrainGenerator& gen)
    {
        for (int x = 0; x < CHUNK_SIZE; x++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                int worldX = position.x + x;
                int worldZ = position.z + z;
                int surfaceHeight = gen.getHeight(worldX, worldZ);
            
                for(int y = 0; y < CHUNK_HEIGHT; y++) {
                    uint8_t type = BlockID::AIR;
                    
                    // 1. Generate SOLID ground
                    if (y < surfaceHeight) {
                        int stoneStartH = Utils::GetRandomNum(3, 7);
                        int mountainStartH = Utils::GetRandomNum(55, 75);
                    
                        if (y < surfaceHeight - stoneStartH) {
                            type = BlockID::STONE;
                        } else if (y < surfaceHeight - 1) {
                            type = BlockID::DIRT;
                        } else {
                            // Top layer: Use Sand if under water, Grass if above
                            if (y < waterLevel) {
                                type = BlockID::SAND; 
                            } else if (surfaceHeight > mountainStartH) {
                                type = BlockID::STONE;
                            } else {
                                type = BlockID::GRASS;
                            }
                        }
                    } 
                    // 2. Generate WATER in the gaps
                    else if (y < waterLevel) {
                        type = BlockID::WATER;
                    }
                
                    blocks[x][y][z] = type;
                }
            }
        }
    }
    void Chunk::GenerateTrees(TerrainGenerator& gen, World* world)
    {
        int step = 4; // minimum 4 blocks apart
        for (int x = 0; x < CHUNK_SIZE; x += step) {
            for (int z = 0; z < CHUNK_SIZE; z += step) {
                int worldX = position.x + x;
                int worldZ = position.z + z;
                int y = gen.getHeight(worldX, worldZ);
            
                if (blocks[x][y-1][z] == BlockID::GRASS) {
                    if (Utils::GetRandomNum(0, 100) < 3) { // 3% chance per step
                        world->AddTree(worldX, y, worldZ);
                    }
                }
            }
        }
        tressGenerated = true;
    }
    void Chunk::GenerateData(TerrainGenerator& gen, World* world) 
    {
        GenerateTerrain(gen);
        if(!tressGenerated) GenerateTrees(gen, world); // there is bug, even tho tress are generated its stil genarte like this bool dont hcnage at all
    }
    void Chunk::GenerateMesh(World* world) {
        // pack this
        std::vector<uint32_t> vertices;
        std::vector<uint32_t> transparentVertices;
        vertices.reserve(CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE * 6);
        transparentVertices.reserve(CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE);

        Chunk* nxp = world->GetChunk({position.x + CHUNK_SIZE, 0, position.z});
        Chunk* nxn = world->GetChunk({position.x - CHUNK_SIZE, 0, position.z});
        Chunk* nzp = world->GetChunk({position.x, 0, position.z + CHUNK_SIZE});
        Chunk* nzn = world->GetChunk({position.x, 0, position.z - CHUNK_SIZE});

        for (int x = 0; x < CHUNK_SIZE; x++) {
            for (int y = 0; y < CHUNK_HEIGHT; y++) {
                for (int z = 0; z < CHUNK_SIZE; z++) {
                    uint8_t blockType = blocks[x][y][z];
                    if (blockType == 0) continue;

                    const BlockData& bData = BlockDatabase::Get(static_cast<BlockID>(blockType));
                    std::vector<uint32_t>& targetVertices = (bData.type == BlockType::LIQUID || bData.type == BlockType::TRANSPARENT) ? transparentVertices : vertices;

                    auto getB = [&](int ox, int oy, int oz) -> uint8_t {
                        int nx = x + ox;
                        int ny = y + oy;
                        int nz = z + oz;

                        // If we look BELOW the bottom of the world, return a solid "Boundary" block
                        if (ny < 0) return e::BlockID::STONE; // Or a specific BEDROCK ID

                        // If we look ABOVE the sky, it's definitely AIR
                        if (ny >= CHUNK_HEIGHT) return e::BlockID::AIR;

                        if (nx >= 0 && nx < CHUNK_SIZE && nz >= 0 && nz < CHUNK_SIZE) 
                            return blocks[nx][ny][nz];

                        // Chunk boundary checks
                        if (nx >= CHUNK_SIZE) return nxp ? nxp->blocks[0][ny][nz] : e::BlockID::AIR;
                        if (nx < 0)           return nxn ? nxn->blocks[CHUNK_SIZE - 1][ny][nz] : e::BlockID::AIR;
                        if (nz >= CHUNK_SIZE) return nzp ? nzp->blocks[nx][ny][0] : e::BlockID::AIR;
                        if (nz < 0)           return nzn ? nzn->blocks[nx][ny][CHUNK_SIZE - 1] : e::BlockID::AIR;

                        return e::BlockID::AIR;
                    };

                    // Rule: Render face if:
                    // 1. Neighbor is AIR
                    // 2. Current is Opaque, neighbor is NOT Opaque
                    // 3. Both are transparent, but different IDs (water next to glass)
                    auto shouldRender = [&](uint8_t neighborId) -> bool {
                        if (neighborId == e::BlockID::AIR) return true;
                        
                        const BlockData& nData = BlockDatabase::Get(static_cast<BlockID>(neighborId));
                        
                        if (bData.isOpaque && !nData.isOpaque) return true;
                        if (!bData.isOpaque && !nData.isOpaque && bData.id != nData.id) return true;

                        return false;
                    };

                    uint8_t topNeighbor = getB(0, 1, 0);
                    uint8_t bottomNeighbor = getB(0, -1, 0);
                    uint8_t zNeighbor = getB(0, 0, 1);
                    uint8_t nzNeighbor = getB(0, 0, -1);
                    uint8_t xNeighbor = getB(1, 0, 0);
                    uint8_t nxNeighbor = getB(-1, 0, 0);
                    
                    float ao[4];
                    if (shouldRender(topNeighbor)) {
                        ao[0] = vertexAO(getB(-1, 1, 0), getB(0, 1, -1), getB(-1, 1, -1));
                        ao[1] = vertexAO(getB(-1, 1, 0), getB(0, 1, 1), getB(-1, 1, 1));
                        ao[2] = vertexAO(getB(1, 1, 0), getB(0, 1, 1), getB(1, 1, 1));
                        ao[3] = vertexAO(getB(1, 1, 0), getB(0, 1, -1), getB(1, 1, -1));
                        Utils::addPackedFace(targetVertices, x, y, z, FaceDirection::TOP, ao, blockType);
                    }
                    if (shouldRender(bottomNeighbor)) {
                        ao[0] = vertexAO(getB(-1, -1, 0), getB(0, -1, -1), getB(-1, -1, -1));
                        ao[1] = vertexAO(getB(1, -1, 0), getB(0, -1, -1), getB(1, -1, -1));
                        ao[2] = vertexAO(getB(1, -1, 0), getB(0, -1, 1), getB(1, -1, 1));
                        ao[3] = vertexAO(getB(-1, -1, 0), getB(0, -1, 1), getB(-1, -1, 1));
                        Utils::addPackedFace(targetVertices, x, y, z, FaceDirection::BOTTOM, ao, blockType);
                    }
                    if (shouldRender(zNeighbor)) {
                        ao[0] = vertexAO(getB(-1, 0, 1), getB(0, -1, 1), getB(-1, -1, 1));
                        ao[1] = vertexAO(getB(1, 0, 1), getB(0, -1, 1), getB(1, -1, 1));
                        ao[2] = vertexAO(getB(1, 0, 1), getB(0, 1, 1), getB(1, 1, 1));
                        ao[3] = vertexAO(getB(-1, 0, 1), getB(0, 1, 1), getB(-1, 1, 1));
                        Utils::addPackedFace(targetVertices, x, y, z, FaceDirection::FRONT, ao, blockType);
                    }
                    if (shouldRender(nzNeighbor)) {
                        ao[0] = vertexAO(getB(1, 0, -1), getB(0, -1, -1), getB(1, -1, -1));
                        ao[1] = vertexAO(getB(-1, 0, -1), getB(0, -1, -1), getB(-1, -1, -1));
                        ao[2] = vertexAO(getB(-1, 0, -1), getB(0, 1, -1), getB(-1, 1, -1));
                        ao[3] = vertexAO(getB(1, 0, -1), getB(0, 1, -1), getB(1, 1, -1));
                        Utils::addPackedFace(targetVertices, x, y, z, FaceDirection::BACK, ao, blockType);
                    }
                    if (shouldRender(nxNeighbor)) {
                        ao[0] = vertexAO(getB(-1, -1, 0), getB(-1, 0, -1), getB(-1, -1, -1));
                        ao[1] = vertexAO(getB(-1, -1, 0), getB(-1, 0, 1), getB(-1, -1, 1));
                        ao[2] = vertexAO(getB(-1, 1, 0), getB(-1, 0, 1), getB(-1, 1, 1));
                        ao[3] = vertexAO(getB(-1, 1, 0), getB(-1, 0, -1), getB(-1, 1, -1));
                        Utils::addPackedFace(targetVertices, x, y, z, FaceDirection::LEFT, ao, blockType);
                    }
                    if (shouldRender(xNeighbor)) {
                        ao[0] = vertexAO(getB(1, -1, 0), getB(1, 0, 1), getB(1, -1, 1));
                        ao[1] = vertexAO(getB(1, -1, 0), getB(1, 0, -1), getB(1, -1, -1));
                        ao[2] = vertexAO(getB(1, 1, 0), getB(1, 0, -1), getB(1, 1, -1));
                        ao[3] = vertexAO(getB(1, 1, 0), getB(1, 0, 1), getB(1, 1, 1));
                        Utils::addPackedFace(targetVertices, x, y, z, FaceDirection::RIGHT, ao, blockType);
                    }
                }
            }
        }

        vertexCount = (int)(vertices.size()); 
        if (vertexCount > 0) {
            vbo->SetData(vertices.data(), (uint32_t)(vertices.size() * sizeof(uint32_t)));
        }

        transparentVertexCount = (int)(transparentVertices.size());
        if (transparentVertexCount > 0) {
            transparentVbo->SetData(transparentVertices.data(), (uint32_t)(transparentVertices.size() * sizeof(uint32_t)));
        }
    }

    uint8_t World::GetBlock(int x, int y, int z)
    {
        if (y < 0 || y >= CHUNK_HEIGHT) return 0;
        int cx = (int)floor((float)x / CHUNK_SIZE) * CHUNK_SIZE;
        int cz = (int)floor((float)z / CHUNK_SIZE) * CHUNK_SIZE;
        auto it = m_Chunks.find({cx, 0, cz});
        return (it != m_Chunks.end()) ? it->second.blocks[x - cx][y][z - cz] : 0;
    }
    void World::SetBlock(int x, int y, int z, uint8_t type)
    {
        if (y < 0 || y >= CHUNK_HEIGHT) return;
        int cx = (int)floor((float)x / CHUNK_SIZE) * CHUNK_SIZE;
        int cz = (int)floor((float)z / CHUNK_SIZE) * CHUNK_SIZE;
        auto it = m_Chunks.find({cx, 0, cz});
        if (it != m_Chunks.end()) {
            int lx = x - cx;
            int lz = z - cz;
            if (it->second.blocks[lx][y][lz] != type) {
                it->second.blocks[lx][y][lz] = type;
                it->second.dirty = true;
                if(!it->second.isGenerating)
                    it->second.GenerateMesh(this);

                // Regenerate neighbors if on edge
                if (lx == 0) { Chunk* c = GetChunk({cx - CHUNK_SIZE, 0, cz}); if(c) c->GenerateMesh(this); }
                if (lx == CHUNK_SIZE - 1) { Chunk* c = GetChunk({cx + CHUNK_SIZE, 0, cz}); if(c) c->GenerateMesh(this); }
                if (lz == 0) { Chunk* c = GetChunk({cx, 0, cz - CHUNK_SIZE}); if(c) c->GenerateMesh(this); }
                if (lz == CHUNK_SIZE - 1) { Chunk* c = GetChunk({cx, 0, cz + CHUNK_SIZE}); if(c) c->GenerateMesh(this); }
            }
        }
    }
    bool World::SetBlockData(int x, int y, int z, uint8_t type)
    {
        if (y < 0 || y >= CHUNK_HEIGHT) return false;
        int cx = (int)floor((float)x / CHUNK_SIZE) * CHUNK_SIZE;
        int cz = (int)floor((float)z / CHUNK_SIZE) * CHUNK_SIZE; 

        Chunk* chunk = GetChunk({cx, 0, cz});
        if (!chunk) return false; // Neighbor not loaded yet      

        chunk->blocks[x - cx][y][z - cz] = type;
        chunk->dirty = true;
        return true;
    }
    
    void World::SaveChunkToDisk(const Chunk& chunk)
    {
        if (!chunk.dirty) return; // ONLY SAVE IF MODIFIED

        std::string filename = m_SavePath + "/chunk_" + std::to_string(chunk.position.x) + "_" + std::to_string(chunk.position.z) + ".dat";
        std::ofstream out(filename, std::ios::binary);
        if (out) {
            out.write((char*)chunk.blocks, CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE);
        }
    }

    void World::UnloadChunks(const glm::vec3& cameraPos, float renderDistance) {
        float buffer = CHUNK_SIZE * 4.0f; // Increased buffer to prevent thrashing
        float threshold = renderDistance + buffer;
        float thresholdSq = threshold * threshold;      

        int count = 0;
        int maxUnloadsPerFrame = 1; // Limit unloads per frame to avoid disk I/O spikes

        for (auto it = m_Chunks.begin(); it != m_Chunks.end(); ) {
            glm::vec3 chunkPos = glm::vec3(it->first.x, 0, it->first.z);
            float distSq = glm::distance2(glm::vec2(cameraPos.x, cameraPos.z), 
                                          glm::vec2(chunkPos.x, chunkPos.z));       
            if (distSq > thresholdSq) {
                SaveChunkToDisk(it->second);
                it->second.DeleteBuffers();
                it = m_Chunks.erase(it);
                count++;
                if (count >= maxUnloadsPerFrame) break; 
            } else {
                ++it;
            }
        }
    }

    void World::Update(const glm::vec3& cameraPos, float renderDistance)
    {     
        int camChunkX = (int)floor(cameraPos.x / CHUNK_SIZE);
        int camChunkZ = (int)floor(cameraPos.z / CHUNK_SIZE);
        int chunkRange = (int)ceil(renderDistance / CHUNK_SIZE);

        // Process unloads every frame but rate-limited
        UnloadChunks(cameraPos, renderDistance);

        // Load new chunks in a spiral pattern around the camera
        int loadsThisFrame = 0;
        int maxLoadsPerFrame = 1; // Limit loads per frame to prevent stuttering
        
        bool budgeted = false;
        for (int r = 0; r <= chunkRange; r++) {
            for (int x = -r; x <= r; x++) {
                for (int z = -r; z <= r; z++) {
                    if (abs(x) != r && abs(z) != r) continue;
                    glm::ivec3 pos((camChunkX + x) * CHUNK_SIZE, 0, (camChunkZ + z) * CHUNK_SIZE);
                    if (m_Chunks.find(pos) == m_Chunks.end()) {
                        LoadChunk(pos);
                        m_Chunks[pos].GenerateMesh(this);
                        glm::ivec3 neighbors[] = {
                            {pos.x + CHUNK_SIZE, 0, pos.z}, {pos.x - CHUNK_SIZE, 0, pos.z},
                            {pos.x, 0, pos.z + CHUNK_SIZE}, {pos.x, 0, pos.z - CHUNK_SIZE}
                        };
                        for (const auto& nPos : neighbors) {
                            if (m_Chunks.count(nPos)) m_Chunks[nPos].GenerateMesh(this);
                        }
                        
                        loadsThisFrame++;
                        if (loadsThisFrame >= maxLoadsPerFrame) {
                            budgeted = true;
                            break;
                        }
                    }
                }
                if (budgeted) break;
            }
            if (budgeted) break;
        }
    }

    void World::GenerateWorld(int worldSizeInChunks)
    {
        for (int x = -worldSizeInChunks / 2; x < worldSizeInChunks / 2; x++) {
            for (int z = -worldSizeInChunks / 2; z < worldSizeInChunks / 2; z++) {
                LoadChunk(glm::ivec3(x * CHUNK_SIZE, 0, z * CHUNK_SIZE));
            }
        }
        for (auto& pair : m_Chunks) pair.second.GenerateMesh(this);
    }

    void World::SaveToFile(const std::string& filename)
    {
        std::ofstream out(filename, std::ios::binary);
        if (!out) return;
        uint32_t chunkCount = (uint32_t)m_Chunks.size();
        out.write((char*)&chunkCount, sizeof(uint32_t));
        for (auto& [pos, chunk] : m_Chunks) {
            out.write((char*)&pos, sizeof(glm::ivec3));
            out.write((char*)chunk.blocks, CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE);
        }
    }

    bool World::LoadFromFile(const std::string& filename)
    {
        std::ifstream in(filename, std::ios::binary);
        if (!in) return false;
        uint32_t chunkCount;
        in.read((char*)&chunkCount, sizeof(uint32_t));
        for (uint32_t i = 0; i < chunkCount; ++i) {
            glm::ivec3 pos;
            in.read((char*)&pos, sizeof(glm::ivec3));
            if (m_Chunks.find(pos) == m_Chunks.end()) {
                Chunk& newChunk = m_Chunks[pos];
                newChunk.position = pos;

                // Opaque
                newChunk.vao = std::make_shared<VertexArray>();
                newChunk.vbo = std::make_shared<VertexBuffer>(nullptr, CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE * 6 * sizeof(uint32_t));
                newChunk.vbo->SetLayout({{ShaderDataType::UInt, "a_Data"}});
                newChunk.vao->AddVertexBuffer(newChunk.vbo);

                // Transparent
                newChunk.transparentVao = std::make_shared<VertexArray>();
                newChunk.transparentVbo = std::make_shared<VertexBuffer>(nullptr, CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE * 6 * sizeof(uint32_t) / 4);
                newChunk.transparentVbo->SetLayout({{ShaderDataType::UInt, "a_Data"}});
                newChunk.transparentVao->AddVertexBuffer(newChunk.transparentVbo);
            }
            in.read((char*)m_Chunks[pos].blocks, CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE);
        }
        for (auto& pair : m_Chunks) pair.second.GenerateMesh(this);
        return true;
    }

    void World::LoadChunk(const glm::ivec3& chunkPos)
    {
        // Already loaded? skip
        if (m_Chunks.find(chunkPos) != m_Chunks.end()) return;

        // Insert a chunk in the map first, get a reference
        Chunk& chunk = m_Chunks[chunkPos];
        chunk.position = chunkPos;

        // Setup VAO/VBO for the chunk (Opaque)
        chunk.vao = std::make_shared<VertexArray>();
        chunk.vbo = std::make_shared<VertexBuffer>(
            nullptr, CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE * 6 * sizeof(uint32_t)
        );
        chunk.vbo->SetLayout({{ShaderDataType::UInt, "a_Data"}});
        chunk.vao->AddVertexBuffer(chunk.vbo);

        // Setup VAO/VBO for the chunk (Transparent)
        chunk.transparentVao = std::make_shared<VertexArray>();
        chunk.transparentVbo = std::make_shared<VertexBuffer>(
            nullptr, CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE * 6 * sizeof(uint32_t) / 4
        );
        chunk.transparentVbo->SetLayout({{ShaderDataType::UInt, "a_Data"}});
        chunk.transparentVao->AddVertexBuffer(chunk.transparentVbo);

        // Try to load from disk
        std::string filename = m_SavePath + "/chunk_" + std::to_string(chunkPos.x) + "_" + std::to_string(chunkPos.z) + ".dat";
        std::ifstream in(filename, std::ios::binary);

        if (in) {
            in.read((char*)chunk.blocks, CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE);
        } else {
            // Mark as generating to prevent SetBlock from regenerating mesh mid-tree
            chunk.isGenerating = true;
            // Generate terrain + trees in the chunk
            chunk.GenerateData(m_Gen, this);
            chunk.isGenerating = false;
            // Generate mesh once at the end
            chunk.GenerateMesh(this);
        }
    }
    void World::MarkChunkAffected(int x, int y, int z, std::unordered_set<Chunk*>& set)
    {
        int chunkX = (int)floor((float)x / CHUNK_SIZE) * CHUNK_SIZE;
        int chunkZ = (int)floor((float)z / CHUNK_SIZE) * CHUNK_SIZE;

        Chunk* chunk = GetChunk({chunkX, 0, chunkZ});
        if (chunk)
        {
            set.insert(chunk);
        }
    }
    void World::UnloadChunk(const glm::ivec3& chunkPos) { 
        auto it = m_Chunks.find(chunkPos);
        if (it != m_Chunks.end()) {
            SaveChunkToDisk(it->second);
            m_Chunks.erase(it); 
        }
    }
    void World::UnloadAllChunks() { 
        for (auto& [pos, chunk] : m_Chunks) {
            SaveChunkToDisk(chunk);
        }
        m_Chunks.clear(); 
    }

    void World::Draw(const std::shared_ptr<Shader>& shader, const glm::vec3& cameraPos, const glm::vec3& cameraDir, float renderDistance)
    {   
        float renderDistSq = renderDistance * renderDistance;
        glm::vec3 look = glm::normalize(cameraDir);

        // Pass 1: Opaque Blocks
        for (auto& pair : m_Chunks) {
            Chunk& chunk = pair.second;

            glm::vec3 chunkCenter = glm::vec3(chunk.position.x + CHUNK_SIZE / 2.0f, CHUNK_HEIGHT / 2.0f, chunk.position.z + CHUNK_SIZE / 2.0f);
            glm::vec3 v = chunkCenter - cameraPos;
            if (glm::dot(v, look) < -CHUNK_HEIGHT) continue; 

            float distSq = glm::dot(cameraPos - chunkCenter, cameraPos - chunkCenter);

            if (distSq < renderDistSq && chunk.vertexCount > 0) {
                shader->SetUniformFloat3("u_ChunkPos", glm::vec3(chunk.position));
                Renderer::Draw(chunk.vao, (uint32_t)chunk.vertexCount);
            }
        }

        // Pass 2: Transparent Blocks (Water, Glass, Leaves)
        // Optimization: In a real engine, you'd sort these back-to-front
        for (auto& pair : m_Chunks) {
            Chunk& chunk = pair.second;

            glm::vec3 chunkCenter = glm::vec3(chunk.position.x + CHUNK_SIZE / 2.0f, CHUNK_HEIGHT / 2.0f, chunk.position.z + CHUNK_SIZE / 2.0f);
            glm::vec3 v = chunkCenter - cameraPos;
            if (glm::dot(v, look) < -CHUNK_HEIGHT) continue;

            float distSq = glm::dot(cameraPos - chunkCenter, cameraPos - chunkCenter);

            if (distSq < renderDistSq && chunk.transparentVertexCount > 0) {
                shader->SetUniformFloat3("u_ChunkPos", glm::vec3(chunk.position));
                Renderer::Draw(chunk.transparentVao, (uint32_t)chunk.transparentVertexCount);
            }
        }
    }

    void World::DrawShadows(const std::shared_ptr<Shader>& shadowShader, const glm::vec3& cameraPos, float renderDistance)
    {
        float renderDistSq = renderDistance * renderDistance;

        for (auto& pair : m_Chunks) {
            Chunk& chunk = pair.second;

            glm::vec3 chunkCenter = glm::vec3(chunk.position.x + CHUNK_SIZE / 2.0f, CHUNK_HEIGHT / 2.0f, chunk.position.z + CHUNK_SIZE / 2.0f);
            float distSq = glm::distance2(glm::vec2(cameraPos.x, cameraPos.z), glm::vec2(chunkCenter.x, chunkCenter.z));

            // We include transparent geometry in shadow pass so leaves and glass can cast shadows
            if (distSq < renderDistSq) {
                if (chunk.vertexCount > 0) {
                    shadowShader->SetUniformFloat3("u_ChunkPos", glm::vec3(chunk.position));
                    Renderer::Draw(chunk.vao, (uint32_t)chunk.vertexCount);
                }
                if (chunk.transparentVertexCount > 0) {
                    shadowShader->SetUniformFloat3("u_ChunkPos", glm::vec3(chunk.position));
                    Renderer::Draw(chunk.transparentVao, (uint32_t)chunk.transparentVertexCount);
                }
            }
        }
    }

    void World::AddTree(int x, int y, int z)
    {
        std::unordered_set<Chunk*> chunksAffected; // for chunks that tree is growing on
        
        int chunkX = (int)floor(x / CHUNK_SIZE) * CHUNK_SIZE;
        int chunkZ = (int)floor(z / CHUNK_SIZE) * CHUNK_SIZE;

        for (Chunk* ch : chunksAffected)
            ch->isGenerating = true;


        int treeHeight = e::Utils::GetRandomNum(3, 6);
        for(int i = 0; i < treeHeight; i++)
        {
            SetBlockData(x, y+i, z, BlockID::OAK_LOG);
        }
        
        // Place leaves
        int canopyHeight = 3;
        for(int i = 0; i < canopyHeight; i++)
        {
            int radius = canopyHeight - i;
            for(int xa = -radius; xa <= radius; xa++)
            {
                for(int za = -radius; za <= radius; za++)
                {
                    if(xa == 0 && za == 0 && i < canopyHeight - 1) continue;

                    int bx = x + xa;
                    int by = y + treeHeight - canopyHeight + i;
                    int bz = z + za;

                    SetBlockData(bx, by, bz, BlockID::OAK_LEAVES);
                    MarkChunkAffected(bx, by, bz, chunksAffected);
                }
            }
        }
        for (Chunk* ch : chunksAffected)
        {
            ch->isGenerating = false;
            ch->GenerateMesh(this);
        }
    }

    Chunk* World::GetChunk(const glm::ivec3& chunkPos)
    {
        auto it = m_Chunks.find(chunkPos);
        return (it != m_Chunks.end()) ? &it->second : nullptr;
    }

    RaycastResult World::Raycast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance)
    {
        glm::vec3 dir = glm::normalize(direction);
        glm::ivec3 currentPos = glm::ivec3(glm::floor(origin));
    
        glm::vec3 deltaDist(
            dir.x != 0 ? glm::abs(1.0f / dir.x) : 1e30f,
            dir.y != 0 ? glm::abs(1.0f / dir.y) : 1e30f,
            dir.z != 0 ? glm::abs(1.0f / dir.z) : 1e30f
        );
    
        glm::ivec3 step;
        glm::vec3 sideDist;
    
        for (int i = 0; i < 3; i++) {
            if (dir[i] < 0) {
                step[i]     = -1;
                sideDist[i] = (origin[i] - currentPos[i]) * deltaDist[i];
            } else {
                step[i]     = 1;
                sideDist[i] = (currentPos[i] + 1.0f - origin[i]) * deltaDist[i];
            }
        }
    
        glm::ivec3 lastNormal(0);
    
        while (true) {
            // True distance check
            float dist = glm::length(glm::vec3(currentPos) + 0.5f - origin);
            if (dist > maxDistance) break;
        
            int blockId = GetBlock(currentPos.x, currentPos.y, currentPos.z);
            e::BlockData bData = BlockDatabase::Get(static_cast<BlockID>(blockId));

            if (bData.id != BlockID::AIR && bData.type != BlockType::LIQUID)
                return { true, currentPos, -lastNormal };
        
            // Step to nearest voxel boundary
            int axis;
            if      (sideDist.x < sideDist.y && sideDist.x < sideDist.z) axis = 0;
            else if (sideDist.y < sideDist.z)                             axis = 1;
            else                                                           axis = 2;
        
            sideDist[axis]  += deltaDist[axis];
            currentPos[axis] += step[axis];
            lastNormal        = glm::ivec3(0);
            lastNormal[axis]  = step[axis];
        }
    
        return { false, glm::ivec3(0), glm::ivec3(0) };
    }
}