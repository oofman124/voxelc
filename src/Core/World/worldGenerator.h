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

extern AssetManager &assetMgr;

struct WorldGeneratorParams
{
    // Base terrain
    int octaves = 4;
    float basePersistence = 0.9f;
    float baseScale = 0.08f;
    float baseAmplitude = 32.0f;

    // Mountains
    float mountainScale = 0.025f;
    float mountainAmplitude = 48.0f;
    float mountainPower = 2.5f; // Controls sharpness


    // Biome
    float biomeScale = 0.004f;
    float biomeBlend = 0.5f; // Not used yet, for future

    // Overall
    float minHeight = 4.0f;
    float maxHeight = 128.0f;
};

class WorldGenerator
{
public:
    WorldGenerator(WorldGeneratorParams params)
        : baseNoise(1298), mountainNoise(4321), riverNoise(9876), biomeNoise(2468)
    {
        this->params = params;
    }
    WorldGenerator() : WorldGenerator(WorldGeneratorParams()) {}

    WorldGeneratorParams getParams() const { return params; }
    void setParams(const WorldGeneratorParams &newParams) { params = newParams; }

    // Get the world position of a block in the terrain
    glm::vec3 getTerrainPosition(int x, int z, float y = 0.0f) const
    {
        // Clamp y to nearest integer
        float clampedY = std::round(y);
        return glm::vec3(
            static_cast<float>(x),
            clampedY,
            static_cast<float>(z));
    }

    float generateHeight(int x, int z) const
    {
        // --- Biome noise ---
        float biome = biomeNoise.noise(x * params.biomeScale, 0, z * params.biomeScale);
        biome = glm::clamp(biome, 0.0f, 1.0f); // Normalize if needed

        // --- Base terrain ---
        float base = 0;
        float frequency = 1;
        float amplitude = 1;
        float maxValue = 0;
        for (int i = 0; i < params.octaves; i++)
        {
            float sampleX = x * params.baseScale * frequency;
            float sampleZ = z * params.baseScale * frequency;
            base += baseNoise.noise(sampleX, 0, sampleZ) * amplitude;
            maxValue += amplitude;
            amplitude *= params.basePersistence;
            frequency *= 2;
        }
        base = (base / maxValue) * params.baseAmplitude;

        // --- Mountains ---
        float mountain = mountainNoise.noise(x * params.mountainScale, 0, z * params.mountainScale);
        mountain = std::pow(std::max(0.0f, mountain), params.mountainPower) * params.mountainAmplitude;



        // --- Combine with biome influence ---
        // Example: biome blends between two base amplitudes (e.g., plains and hills)
        float biomeBase = glm::mix(params.baseAmplitude * 0.5f, params.baseAmplitude * 1.5f, biome);
        float height = (base / params.baseAmplitude) * biomeBase + mountain;

        // Clamp and round
        height = glm::clamp(height, params.minHeight, params.maxHeight);
        return std::round(height);
    }

    std::shared_ptr<Chunk> generateChunk(std::shared_ptr<Object> parent, int chunkX, int chunkZ, int width, int depth)
    {
        auto chunk = std::make_shared<Chunk>("Chunk_" + std::to_string(chunkX) + "_" + std::to_string(chunkZ));
        chunk->SetParent(parent);
        chunk->setPosition(glm::vec3(chunkX, 0.0f, chunkZ));
        chunk->setState(ChunkState::GENERATING);

        // Generate terrain for each block in the chunk
        for (int x = 0; x < width; x++)
        {
            for (int z = 0; z < depth; z++)
            {
                int worldX = chunkX + x;
                int worldZ = chunkZ + z;
                float height = generateHeight(worldX, worldZ);

                // Generate column of blocks
                for (int y = 0; y <= height; y++)
                {
                    BlockType type;
                    if (y == static_cast<int>(height))
                    {
                        type = BLOCK_TYPE_GRASS; // Top layer
                    }
                    else if (y > height - 4)
                    {
                        type = BLOCK_TYPE_DIRT; // Few layers of dirt
                    }
                    else
                    {
                        type = BLOCK_TYPE_STONE; // Stone base
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
    void setParams(WorldGeneratorParams param) { params = param; }

private:
    PerlinNoise baseNoise;
    PerlinNoise mountainNoise;
    PerlinNoise riverNoise;
    PerlinNoise biomeNoise;
    WorldGeneratorParams params;

    void createBlock(shared_ptr<Object> parent, int x, int z, float height)
    {
        auto block = make_shared<Block>("Block_" + to_string(x) + "_" + to_string(z), glm::vec3(x, height, z), BLOCK_TYPE_GRASS);
        block->SetParent(parent);
    }
};

#endif