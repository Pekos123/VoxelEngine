#include "Block.h"

namespace e {
    std::vector<BlockData> BlockDatabase::m_Blocks;

    void BlockDatabase::Initialize() {
        m_Blocks.resize(static_cast<size_t>(BlockID::COUNT));

        // { ID, TYPE, OPAQUE }

        m_Blocks[static_cast<size_t>(BlockID::AIR)] = { BlockID::AIR, BlockType::TRANSPARENT, false };
        m_Blocks[static_cast<size_t>(BlockID::GRASS)] = { BlockID::GRASS, BlockType::SOLID, true };
        m_Blocks[static_cast<size_t>(BlockID::DIRT)] = { BlockID::DIRT, BlockType::SOLID, true };
        m_Blocks[static_cast<size_t>(BlockID::STONE)] = { BlockID::STONE, BlockType::SOLID, true };
        m_Blocks[static_cast<size_t>(BlockID::OAK_PLANKS)] = { BlockID::OAK_PLANKS, BlockType::SOLID, true };
        m_Blocks[static_cast<size_t>(BlockID::OAK_LOG)] = { BlockID::OAK_LOG, BlockType::SOLID, true };
        m_Blocks[static_cast<size_t>(BlockID::COBBLESTONE)] = { BlockID::COBBLESTONE, BlockType::SOLID, true };
        m_Blocks[static_cast<size_t>(BlockID::GLASS)] = { BlockID::GLASS, BlockType::TRANSPARENT, false };
        m_Blocks[static_cast<size_t>(BlockID::OAK_LEAVES)] = { BlockID::OAK_LEAVES, BlockType::TRANSPARENT, false };
        m_Blocks[static_cast<size_t>(BlockID::WATER)] = { BlockID::WATER, BlockType::LIQUID, false };
        m_Blocks[static_cast<size_t>(BlockID::SANDSTONE)] = { BlockID::SANDSTONE, BlockType::SOLID, true };
        m_Blocks[static_cast<size_t>(BlockID::SAND)] = { BlockID::SAND, BlockType::SOLID, true };
    }
}