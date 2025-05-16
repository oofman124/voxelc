#ifndef WORLD_GENERATOR_H
#define WORLD_GENERATOR_H

#include <memory>
#include <glm/glm.hpp>
#include <cmath>
#include "../Core/object.h"
#include "../Core/assets.h"
#include "chunk.h"
#include "perlinNoise.h"

extern AssetManager& assetMgr;


class WorldGenerator
{
public:
    WorldGenerator()
    {
        octaves = 4;
        persistence = 0.5f;
        scale = 0.1f;
        amplitude = 5.0f;
    }

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

        for (int i = 0; i < octaves; i++) {
            float sampleX = x * scale * frequency;
            float sampleZ = z * scale * frequency;

            total += noise.noise(sampleX, 0, sampleZ) * amplitude;

            maxValue += amplitude;
            amplitude *= persistence;
            frequency *= 2;
        }

        // Normalize, scale, and round to nearest integer
        return std::round((total / maxValue) * this->amplitude);
    }

    void generateChunk(shared_ptr<Object> parent, int startX, int startZ, int width, int depth)
    {
        auto chunk = make_shared<Chunk>("Chunk_" + to_string(startX) + "_" + to_string(startZ));
        chunk->SetParent(parent);
        chunk->setPosition(glm::vec3(startX, 0.0f, startZ));
        for (int x = startX; x < startX + width; x++)
        {
            for (int z = startZ; z < startZ + depth; z++)
            {
                float height = generateHeight(x, z);
                createBlock(chunk, x, z, height);
            }
        }
    }
    // Setters for terrain parameters
    void setOctaves(int o) { octaves = o; }
    void setPersistence(float p) { persistence = p; }
    void setScale(float s) { scale = s; }
    void setAmplitude(float a) { amplitude = a; }

private:
    PerlinNoise noise;
    
    // Terrain generation parameters
    int octaves;        // Number of noise layers
    float persistence;  // How much each octave contributes
    float scale;        // Scale of the noise
    float amplitude;    // Height multiplier

    void createBlock(shared_ptr<Object> parent, int x, int z, float height)
    {
        auto block = make_shared<PVObject>("Block_" + to_string(x) + "_" + to_string(z));
        block->SetParent(parent);

        if (auto transform = block->transform.lock())
        {
            transform->setPosition(getTerrainPosition(x, z, height));
            transform->setScale(glm::vec3(1.0f, 1.0f, 1.0f));
        }

        if (auto meshRenderer = block->meshRenderer.lock())
        {
            meshRenderer->Initialize(
                block->transformPtr,
                Meshes::cube,
                assetMgr.getShader("default"),
                assetMgr.getTexture("grass"),
                MESH_RENDERER_MODE_DEFAULT);
        }
    }
};

#endif