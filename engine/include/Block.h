#ifndef BLOCK_H
#define BLOCK_H

#include <vector>

namespace e
{
    enum BlockID
    {
        AIR = 0,
        GRASS,
        DIRT,
        STONE,
        OAK_PLANKS,
        OAK_LOG,
        COBBLESTONE,
        GLASS,
        OAK_LEAVES,
        WATER,
        SANDSTONE,
        SAND,
        COUNT // ALWAYS LAST - Used to size arrays
    };
    enum BlockType
    {
        SOLID,
        LIQUID,
        TRANSPARENT,
        DECORATION // NO COLLISION, NO OCCLUSION, JUST RENDER
    };

    struct BlockData
    {
        BlockID id;
        BlockType type;
        bool isOpaque;
    };

    class BlockDatabase {
    public:
        // static cast for example: const BlockData& bData = BlockDatabase::Get(static_cast<BlockID>(blockType));
        static const BlockData& Get(BlockID id) {
            size_t index = static_cast<size_t>(id);
            if (index >= m_Blocks.size()) {
                // Return AIR data as a fallback to prevent crash
                return m_Blocks[0];
            }
            return m_Blocks[index];
        }
        static void Initialize();
    private:
        static std::vector<BlockData> m_Blocks;
    };
}
#endif // BLOCK_H