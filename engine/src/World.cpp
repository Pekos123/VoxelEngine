#include <World.h>

namespace e
{
    static float vertexAO(bool side1, bool side2, bool corner) {
        if (side1 && side2) return 0.0f;
        return (3.0f - (float(side1) + float(side2) + float(corner))) / 3.0f;
    }

    void Chunk::GenerateData(TerrainGenerator& gen) {
        for (int x = 0; x < CHUNK_SIZE; x++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                int worldX = position.x + x;
                int worldZ = position.z + z;
                int surfaceHeight = gen.getHeight(worldX, worldZ);
                for (int y = 0; y < CHUNK_HEIGHT; y++) {
                    uint8_t type = 0;
                    if (y < surfaceHeight - 4) type = 3; // Stone
                    else if (y < surfaceHeight - 1) type = 2; // Dirt
                    else if (y < surfaceHeight) type = 1; // Grass
                    blocks[x][y][z] = type;
                }
            }
        }
    }

    void Chunk::GenerateMesh(World* world) {
        // pack this
        std::vector<uint32_t> vertices;
        vertices.reserve(CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE * 6);

        Chunk* nxp = world->GetChunk({position.x + CHUNK_SIZE, 0, position.z});
        Chunk* nxn = world->GetChunk({position.x - CHUNK_SIZE, 0, position.z});
        Chunk* nzp = world->GetChunk({position.x, 0, position.z + CHUNK_SIZE});
        Chunk* nzn = world->GetChunk({position.x, 0, position.z - CHUNK_SIZE});

        for (int x = 0; x < CHUNK_SIZE; x++) {
            for (int y = 0; y < CHUNK_HEIGHT; y++) {
                for (int z = 0; z < CHUNK_SIZE; z++) {
                    uint8_t blockType = blocks[x][y][z];
                    if (blockType == 0) continue;

                    auto getB = [&](int ox, int oy, int oz) -> bool {
                        int nx = x + ox;
                        int ny = y + oy;
                        int nz = z + oz;
                        if (ny < 0 || ny >= CHUNK_HEIGHT) return false;
                        if (nx >= 0 && nx < CHUNK_SIZE && nz >= 0 && nz < CHUNK_SIZE) return blocks[nx][ny][nz] != 0;
                        if (nx >= CHUNK_SIZE) return nxp ? nxp->blocks[0][ny][nz] != 0 : false;
                        if (nx < 0)           return nxn ? nxn->blocks[CHUNK_SIZE - 1][ny][nz] != 0 : false;
                        if (nz >= CHUNK_SIZE) return nzp ? nzp->blocks[nx][ny][0] != 0 : false;
                        if (nz < 0)           return nzn ? nzn->blocks[nx][ny][CHUNK_SIZE - 1] != 0 : false;
                        return false;
                    };
                    float ao[4];
                    if (!getB(0, 1, 0)) {
                        ao[0] = vertexAO(getB(-1, 1, 0), getB(0, 1, -1), getB(-1, 1, -1));
                        ao[1] = vertexAO(getB(-1, 1, 0), getB(0, 1, 1), getB(-1, 1, 1));
                        ao[2] = vertexAO(getB(1, 1, 0), getB(0, 1, 1), getB(1, 1, 1));
                        ao[3] = vertexAO(getB(1, 1, 0), getB(0, 1, -1), getB(1, 1, -1));
                        Utils::addPackedFace(vertices, x, y, z, FaceDirection::TOP, ao, blockType);
                    }
                    if (!getB(0, -1, 0)) {
                        ao[0] = vertexAO(getB(-1, -1, 0), getB(0, -1, -1), getB(-1, -1, -1));
                        ao[1] = vertexAO(getB(1, -1, 0), getB(0, -1, -1), getB(1, -1, -1));
                        ao[2] = vertexAO(getB(1, -1, 0), getB(0, -1, 1), getB(1, -1, 1));
                        ao[3] = vertexAO(getB(-1, -1, 0), getB(0, -1, 1), getB(-1, -1, 1));
                        Utils::addPackedFace(vertices, x, y, z, FaceDirection::BOTTOM, ao, blockType);
                    }
                    if (!getB(0, 0, 1)) {
                        ao[0] = vertexAO(getB(-1, 0, 1), getB(0, -1, 1), getB(-1, -1, 1));
                        ao[1] = vertexAO(getB(1, 0, 1), getB(0, -1, 1), getB(1, -1, 1));
                        ao[2] = vertexAO(getB(1, 0, 1), getB(0, 1, 1), getB(1, 1, 1));
                        ao[3] = vertexAO(getB(-1, 0, 1), getB(0, 1, 1), getB(-1, 1, 1));
                        Utils::addPackedFace(vertices, x, y, z, FaceDirection::FRONT, ao, blockType);
                    }
                    if (!getB(0, 0, -1)) {
                        ao[0] = vertexAO(getB(1, 0, -1), getB(0, -1, -1), getB(1, -1, -1));
                        ao[1] = vertexAO(getB(-1, 0, -1), getB(0, -1, -1), getB(-1, -1, -1));
                        ao[2] = vertexAO(getB(-1, 0, -1), getB(0, 1, -1), getB(-1, 1, -1));
                        ao[3] = vertexAO(getB(1, 0, -1), getB(0, 1, -1), getB(1, 1, -1));
                        Utils::addPackedFace(vertices, x, y, z, FaceDirection::BACK, ao, blockType);
                    }
                    if (!getB(-1, 0, 0)) {
                        ao[0] = vertexAO(getB(-1, -1, 0), getB(-1, 0, -1), getB(-1, -1, -1));
                        ao[1] = vertexAO(getB(-1, -1, 0), getB(-1, 0, 1), getB(-1, -1, 1));
                        ao[2] = vertexAO(getB(-1, 1, 0), getB(-1, 0, 1), getB(-1, 1, 1));
                        ao[3] = vertexAO(getB(-1, 1, 0), getB(-1, 0, -1), getB(-1, 1, -1));
                        Utils::addPackedFace(vertices, x, y, z, FaceDirection::LEFT, ao, blockType);
                    }
                    if (!getB(1, 0, 0)) {
                        ao[0] = vertexAO(getB(1, -1, 0), getB(1, 0, 1), getB(1, -1, 1));
                        ao[1] = vertexAO(getB(1, -1, 0), getB(1, 0, -1), getB(1, -1, -1));
                        ao[2] = vertexAO(getB(1, 1, 0), getB(1, 0, -1), getB(1, 1, -1));
                        ao[3] = vertexAO(getB(1, 1, 0), getB(1, 0, 1), getB(1, 1, 1));
                        Utils::addPackedFace(vertices, x, y, z, FaceDirection::RIGHT, ao, blockType);
                    }
                }
            }
        }

        vertexCount = (int)(vertices.size()); 
        if (vertexCount > 0) {
            vbo->SetData(vertices.data(), (uint32_t)(vertices.size() * sizeof(uint32_t)));
        }
    }

    uint8_t World::GetBlock(int x, int y, int z)
    {
        if (y < 0 || y >= CHUNK_HEIGHT) return 0;
        int cx = (int)floor((float)x / CHUNK_SIZE) * CHUNK_SIZE;
        int cz = (int)floor((float)z / CHUNK_SIZE) * CHUNK_SIZE;
        auto it = chunks.find({cx, 0, cz});
        return (it != chunks.end()) ? it->second.blocks[x - cx][y][z - cz] : 0;
    }

    void World::SetBlock(int x, int y, int z, uint8_t type)
    {
        if (y < 0 || y >= CHUNK_HEIGHT) return;
        int cx = (int)floor((float)x / CHUNK_SIZE) * CHUNK_SIZE;
        int cz = (int)floor((float)z / CHUNK_SIZE) * CHUNK_SIZE;
        auto it = chunks.find({cx, 0, cz});
        if (it != chunks.end()) {
            int lx = x - cx;
            int lz = z - cz;
            it->second.blocks[lx][y][lz] = type;
            it->second.GenerateMesh(this);

            // Regenerate neighbors if on edge
            if (lx == 0) { Chunk* c = GetChunk({cx - CHUNK_SIZE, 0, cz}); if(c) c->GenerateMesh(this); }
            if (lx == CHUNK_SIZE - 1) { Chunk* c = GetChunk({cx + CHUNK_SIZE, 0, cz}); if(c) c->GenerateMesh(this); }
            if (lz == 0) { Chunk* c = GetChunk({cx, 0, cz - CHUNK_SIZE}); if(c) c->GenerateMesh(this); }
            if (lz == CHUNK_SIZE - 1) { Chunk* c = GetChunk({cx, 0, cz + CHUNK_SIZE}); if(c) c->GenerateMesh(this); }
        }
    }

    static int updateNum = 0;
    static float timeSinceLastUnload = 0.0f;
    void UnloadChunks(const glm::vec3& cameraPos, float renderDistance, std::unordered_map<glm::ivec3, Chunk>& chunks) {
        float buffer = CHUNK_SIZE * 2.0f;
        float threshold = renderDistance + buffer;
        float thresholdSq = threshold * threshold;      

        for (auto it = chunks.begin(); it != chunks.end(); ) {
            glm::vec3 chunkPos = glm::vec3(it->first.x, 0, it->first.z);

            // Distance squared is much faster (no sqrt)
            float distSq = glm::distance2(glm::vec2(cameraPos.x, cameraPos.z), 
                                          glm::vec2(chunkPos.x, chunkPos.z));       
            if (distSq > thresholdSq) {
                it->second.DeleteBuffers(); // Clean up GPU
                it = chunks.erase(it);      // THIS IS KEY: erase returns the next iterator
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

        timeSinceLastUnload += Renderer::deltaTime;
        if (timeSinceLastUnload > 1.0f) { // Unload chunks every 1 second
            UnloadChunks(cameraPos, renderDistance, chunks);
            timeSinceLastUnload = 0.0f;
        }

        // Load new chunks in a spiral pattern around the camera to minimize pop-in
        bool budgeted = false;
        for (int r = 0; r <= chunkRange; r++) {
            for (int x = -r; x <= r; x++) {
                for (int z = -r; z <= r; z++) {
                    if (abs(x) != r && abs(z) != r) continue;
                    glm::ivec3 pos((camChunkX + x) * CHUNK_SIZE, 0, (camChunkZ + z) * CHUNK_SIZE);
                    if (chunks.find(pos) == chunks.end()) {
                        LoadChunk(pos);
                        chunks[pos].GenerateMesh(this);
                        glm::ivec3 neighbors[] = {
                            {pos.x + CHUNK_SIZE, 0, pos.z}, {pos.x - CHUNK_SIZE, 0, pos.z},
                            {pos.x, 0, pos.z + CHUNK_SIZE}, {pos.x, 0, pos.z - CHUNK_SIZE}
                        };
                        for (const auto& nPos : neighbors) {
                            if (chunks.count(nPos)) chunks[nPos].GenerateMesh(this);
                        }
                        budgeted = true; break;
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
        for (auto& pair : chunks) pair.second.GenerateMesh(this);
    }

    void World::SaveToFile(const std::string& filename)
    {
        std::ofstream out(filename, std::ios::binary);
        if (!out) return;
        uint32_t chunkCount = (uint32_t)chunks.size();
        out.write((char*)&chunkCount, sizeof(uint32_t));
        for (auto& [pos, chunk] : chunks) {
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
            if (chunks.find(pos) == chunks.end()) {
                Chunk newChunk;
                newChunk.position = pos;
                newChunk.vao = std::make_shared<VertexArray>();
                newChunk.vbo = std::make_shared<VertexBuffer>(nullptr, CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE * 6 * sizeof(uint32_t));
                newChunk.vbo->SetLayout({{ShaderDataType::UInt, "a_Data"}});
                newChunk.vao->AddVertexBuffer(newChunk.vbo);
                chunks[pos] = newChunk;
            }
            in.read((char*)chunks[pos].blocks, CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE);
        }
        for (auto& pair : chunks) pair.second.GenerateMesh(this);
        return true;
    }

    void World::LoadChunk(const glm::ivec3& chunkPos)
    {
        if (chunks.find(chunkPos) != chunks.end()) return;
        Chunk newChunk;
        newChunk.position = chunkPos;
        newChunk.vao = std::make_shared<VertexArray>();
        newChunk.vbo = std::make_shared<VertexBuffer>(nullptr, CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE * 6 * sizeof(uint32_t));
        newChunk.vbo->SetLayout({{ShaderDataType::UInt, "a_Data"}});
        newChunk.vao->AddVertexBuffer(newChunk.vbo);
        newChunk.GenerateData(gen);
        chunks[chunkPos] = newChunk;
    }

    void World::UnloadChunk(const glm::ivec3& chunkPos) { chunks.erase(chunkPos); }
    void World::UnloadAllChunks() { chunks.clear(); }

    void World::Draw(const std::shared_ptr<Shader>& shader, const glm::vec3& cameraPos, const glm::vec3& cameraDir, float renderDistance)
    {   
        float renderDistSq = renderDistance * renderDistance;

        // The "Forward" vector of the camera
        glm::vec3 look = glm::normalize(cameraDir);

        for (auto& pair : chunks) {
            Chunk& chunk = pair.second;

            glm::vec3 chunkPos = glm::vec3(chunk.position);
            glm::vec3 chunkCenter = glm::vec3(chunk.position.x + CHUNK_SIZE / 2.0f, CHUNK_HEIGHT / 2.0f, chunk.position.z + CHUNK_SIZE / 2.0f);

            // point to plane
            glm::vec3 v = chunkCenter - cameraPos;
            if (glm::dot(v, look) < -CHUNK_HEIGHT) continue; // Subtract CHUNK_SIZE as a safety margin

            float distSq = glm::dot(cameraPos - chunkCenter, cameraPos - chunkCenter);

            if (distSq < renderDistSq && chunk.vertexCount > 0) {
                shader->SetUniformFloat3("u_ChunkPos", glm::vec3(chunk.position));
                Renderer::Draw(chunk.vao, (uint32_t)chunk.vertexCount);
            }
        }
    }

    Chunk* World::GetChunk(const glm::ivec3& chunkPos)
    {
        auto it = chunks.find(chunkPos);
        return (it != chunks.end()) ? &it->second : nullptr;
    }

    RaycastResult World::Raycast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance)
    {
        glm::vec3 pos = origin;
        glm::vec3 dir = glm::normalize(direction);

        glm::ivec3 currentPos = glm::floor(pos);
        glm::vec3 deltaDist = glm::abs(1.0f / dir);
        
        glm::ivec3 step;
        glm::vec3 sideDist;

        if (dir.x < 0) {
            step.x = -1;
            sideDist.x = (pos.x - currentPos.x) * deltaDist.x;
        } else {
            step.x = 1;
            sideDist.x = (currentPos.x + 1.0f - pos.x) * deltaDist.x;
        }

        if (dir.y < 0) {
            step.y = -1;
            sideDist.y = (pos.y - currentPos.y) * deltaDist.y;
        } else {
            step.y = 1;
            sideDist.y = (currentPos.y + 1.0f - pos.y) * deltaDist.y;
        }

        if (dir.z < 0) {
            step.z = -1;
            sideDist.z = (pos.z - currentPos.z) * deltaDist.z;
        } else {
            step.z = 1;
            sideDist.z = (currentPos.z + 1.0f - pos.z) * deltaDist.z;
        }

        float dist = 0.0f;
        glm::ivec3 lastNormal(0);

        while (dist < maxDistance) {
            if (GetBlock(currentPos.x, currentPos.y, currentPos.z) != 0) {
                return { true, currentPos, -lastNormal };
            }

            if (sideDist.x < sideDist.y) {
                if (sideDist.x < sideDist.z) {
                    dist = sideDist.x;
                    sideDist.x += deltaDist.x;
                    currentPos.x += step.x;
                    lastNormal = glm::ivec3(step.x, 0, 0);
                } else {
                    dist = sideDist.z;
                    sideDist.z += deltaDist.z;
                    currentPos.z += step.z;
                    lastNormal = glm::ivec3(0, 0, step.z);
                }
            } else {
                if (sideDist.y < sideDist.z) {
                    dist = sideDist.y;
                    sideDist.y += deltaDist.y;
                    currentPos.y += step.y;
                    lastNormal = glm::ivec3(0, step.y, 0);
                } else {
                    dist = sideDist.z;
                    sideDist.z += deltaDist.z;
                    currentPos.z += step.z;
                    lastNormal = glm::ivec3(0, 0, step.z);
                }
            }
        }

        return { false, glm::ivec3(0), glm::ivec3(0) };
    }
}