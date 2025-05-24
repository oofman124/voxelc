#ifndef BLOCK_DATABASE_H
#define BLOCK_DATABASE_H
#include <map>
#include <memory>
#include <array>
#include <glm/glm.hpp>
#include "block.h"
#include <unordered_map>

namespace BlockDatabase {
    extern std::map<BlockType, BlockInfo> blocks;
    extern std::unordered_map<BlockType, std::string> BlockNameDatabase;

    void initialize(std::shared_ptr<TextureAtlas> atlas);
    const BlockInfo& getBlockInfo(BlockType type);
    bool isBlockTypeValid(BlockType type);
}

#endif // BLOCK_DATABASE_H