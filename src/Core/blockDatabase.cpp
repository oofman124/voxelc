#include "blockDatabase.h"

namespace BlockDatabase {
    std::map<BlockType, BlockInfo> blocks;
    std::unordered_map<BlockType, std::string> BlockNameDatabase;

    void initialize(std::shared_ptr<TextureAtlas> atlas) {
        // Initialize the block name database
        BlockNameDatabase[BLOCK_TYPE_GRASS] = "Grass Block";
        BlockNameDatabase[BLOCK_TYPE_DIRT] = "Dirt";
        BlockNameDatabase[BLOCK_TYPE_STONE] = "Stone";
        BlockNameDatabase[BLOCK_TYPE_WOOD] = "Wood Planks";

        // Grass Block
        std::array<glm::vec2, 6> grassCoords = {
            glm::vec2(0, 0),  // Top (grass)
            glm::vec2(2, 0),  // Bottom (dirt)
            glm::vec2(3, 0),  // Left (grass side)
            glm::vec2(3, 0),  // Right (grass side)
            glm::vec2(3, 0),  // Front (grass side)
            glm::vec2(3, 0)   // Back (grass side)
        };
        blocks[BLOCK_TYPE_GRASS] = BlockInfo(
            BLOCK_TYPE_GRASS,
            "Grass Block",
            generateBlockMeshFromAtlas(atlas, grassCoords)
        );

        // Dirt Block
        std::array<glm::vec2, 6> dirtCoords = {
            glm::vec2(2, 0),  // All faces use the same dirt texture
            glm::vec2(2, 0),
            glm::vec2(2, 0),
            glm::vec2(2, 0),
            glm::vec2(2, 0),
            glm::vec2(2, 0)
        };
        blocks[BLOCK_TYPE_DIRT] = BlockInfo(
            BLOCK_TYPE_DIRT,
            "Dirt",
            generateBlockMeshFromAtlas(atlas, dirtCoords)
        );

        // Stone Block
        std::array<glm::vec2, 6> stoneCoords = {
            glm::vec2(1, 0),  // All faces use the same stone texture
            glm::vec2(1, 0),
            glm::vec2(1, 0),
            glm::vec2(1, 0),
            glm::vec2(1, 0),
            glm::vec2(1, 0)
        };
        blocks[BLOCK_TYPE_STONE] = BlockInfo(
            BLOCK_TYPE_STONE,
            "Stone",
            generateBlockMeshFromAtlas(atlas, stoneCoords)
        );

        // Wood Planks
        std::array<glm::vec2, 6> planksCoords = {
            glm::vec2(4, 0),  // All faces use the same planks texture
            glm::vec2(4, 0),
            glm::vec2(4, 0),
            glm::vec2(4, 0),
            glm::vec2(4, 0),
            glm::vec2(4, 0)
        };
        blocks[BLOCK_TYPE_WOOD] = BlockInfo(    
            BLOCK_TYPE_WOOD,
            "Wood Planks",
            generateBlockMeshFromAtlas(atlas, planksCoords)
        );
    }

    const BlockInfo& getBlockInfo(BlockType type) {
        auto it = blocks.find(type);
        if (it != blocks.end()) {
            return it->second;
        }
        throw std::runtime_error("Block type not found in database");
    }

    bool isBlockTypeValid(BlockType type) {
        return blocks.find(type) != blocks.end();
    }
}