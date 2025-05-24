#ifndef WORLD_GENERATOR_H
#define WORLD_GENERATOR_H

#include <memory>
#include <glm/glm.hpp>
#include <cmath>
#include "../object.h"
#include "../block.h"
#include "../assets.h"
#include "chunk.h"
#include "perlinNoise.h"

extern AssetManager& assetMgr;


struct WorldGeneratorParams
{
    int octaves = 4;        // Number of noise layers
    float persistence = 0.5f;  // How much each octave contributes
    float scale = 0.1f;        // Scale of the noise
    float amplitude = 64.0f;    // Height multiplier
};



class WorldGenerator
{
public:
    WorldGenerator(WorldGeneratorParams params)
    {
        this->params = params;
    }    
    WorldGenerator() : WorldGenerator(WorldGeneratorParams()) {}

    WorldGeneratorParams getParams() const { return params; }
    void setParams(const WorldGeneratorParams& newParams) { params = newParams; }

    // Get the world position of a block in the terrain
    glm::vec3 getTerrainPosition(int x, int z, float y = 0.0f) const {
        // Clamp y to nearest integer
        float clampedY = std::round(y);
        return glm::vec3(
            static_cast<float>(x),
            clampedY,
            static_cast<float>(z));
    }

    float generateHeight(int x, int z) const {
        float total = 0;
        float frequency = 1;
        float amplitude = 1;
        float maxValue = 0;

        for (int i = 0; i < params.octaves; i++) {
            float sampleX = x * params.scale * frequency;
            float sampleZ = z * params.scale * frequency;

            total += noise.noise(sampleX, 0, sampleZ) * amplitude;

            maxValue += amplitude;
            amplitude *= params.persistence;
            frequency *= 2;
        }

        // Normalize, scale, and round to nearest integer
        return std::round((total / maxValue) * params.amplitude);
    }

    std::shared_ptr<Chunk> generateChunk(std::shared_ptr<Object> parent, int chunkX, int chunkZ, int width, int depth) {
        std::lock_guard<std::mutex> lock(generatorMutex);
        auto chunk = std::make_shared<Chunk>("Chunk_" + std::to_string(chunkX) + "_" + std::to_string(chunkZ));
        chunk->SetParent(parent);
        chunk->setPosition(glm::vec3(chunkX, 0.0f, chunkZ));
        chunk->setState(ChunkState::GENERATING);

        // Generate terrain for each block in the chunk
        for (int x = 0; x < width; x++) {
            for (int z = 0; z < depth; z++) {
                int worldX = chunkX + x;
                int worldZ = chunkZ + z;
                float height = generateHeight(worldX, worldZ);

                // Generate column of blocks
                for (int y = 0; y <= height; y++) {
                    BlockType type;
                    if (y == static_cast<int>(height)) {
                        type = BLOCK_TYPE_GRASS;  // Top layer
                    } else if (y > height - 4) {
                        type = BLOCK_TYPE_DIRT;   // Few layers of dirt
                    } else {
                        type = BLOCK_TYPE_STONE;  // Stone base
                    }
                    
                    // Set block in local chunk coordinates
                    chunk->setBlock(x, y, z, type);
                }
            }
        }

        // Force initial mesh update
        chunk->updateMesh();
        chunk->setState(ChunkState::READY);
        return chunk;
    }
    // Setters for terrain parameters
    void setOctaves(int o) { params.octaves = o; }
    void setPersistence(float p) { params.persistence = p; }
    void setScale(float s) { params.scale = s; }
    void setAmplitude(float a) { params.amplitude = a; }

private:
mutable std::mutex generatorMutex;
    PerlinNoise noise;
    WorldGeneratorParams params;

    void createBlock(shared_ptr<Object> parent, int x, int z, float height)
    {
        auto block = make_shared<Block>("Block_" + to_string(x) + "_" + to_string(z), glm::vec3(x, height, z), BLOCK_TYPE_GRASS);
        block->SetParent(parent);
    }
};

#endif