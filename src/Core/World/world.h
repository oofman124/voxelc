#ifndef WORLD_H
#define WORLD_H

#include <memory>
#include <vector>
#include <unordered_map>
#include <queue>
#include "worldGenerator.h"
#include "../object.h"

// Chunk grid coordinate hasher
struct ChunkCoordHash
{
    std::size_t operator()(const glm::ivec2 &k) const
    {
        return std::hash<int>()(k.x) ^ (std::hash<int>()(k.y) << 1);
    }
};

class World
{
public:
    World() : worldGen()
    {
        root = std::make_shared<Object>("World");
    }
    ~World()
    {
        for (auto &chunk : chunks)
        {
            chunk.second.reset();
        }
        chunks.clear();
        root.reset();
    }

    void requestChunk(int gridX, int gridZ)
    {
        glm::ivec2 coords(gridX, gridZ);

        // Check if chunk already exists or is queued
        if (chunks.find(coords) != chunks.end() ||
            std::find(chunkRequests.begin(), chunkRequests.end(), coords) != chunkRequests.end())
        {
            return;
        }

        chunkRequests.push_back(coords);
    }

    std::shared_ptr<Chunk> getChunk(int gridX, int gridZ) const
    {
        auto it = chunks.find(glm::ivec2(gridX, gridZ));
        return it != chunks.end() ? it->second : nullptr;
    }

    void generateTerrain(int width = 10, int depth = 10)
    {
        // Calculate grid coordinates to center around origin (0,0,0)
        int halfWidth = width / 2;
        int halfDepth = depth / 2;
        int startX = -halfWidth;
        int startZ = -halfDepth;

        // Create temporary vector of coordinates to request
        std::vector<glm::ivec2> coordsToRequest;
        coordsToRequest.reserve(width * depth); // Pre-allocate for performance

        for (int x = 0; x < width; x++)
        {
            for (int z = 0; z < depth; z++)
            {
                int gridX = startX + x;
                int gridZ = startZ + z;
                coordsToRequest.emplace_back(gridX, gridZ);
            }
        }

        for (const auto &coords : coordsToRequest)
        {
            if (chunks.find(coords) != chunks.end() ||
                std::find(chunkRequests.begin(), chunkRequests.end(), coords) != chunkRequests.end())
            {
                continue;
            }
            int worldX = coords.x * Chunk::CHUNK_SIZE;
            int worldZ = coords.y * Chunk::CHUNK_SIZE;
            auto chunk = worldGen.generateChunk(root, worldX, worldZ, Chunk::CHUNK_SIZE, Chunk::CHUNK_SIZE);
            chunks[coords] = chunk;
            // requestChunk(coords.x, coords.y);
        }
    }

    void update()
    {
        while (!chunkRequests.empty() && chunksInGeneration < maxConcurrentGeneration)
        {
            glm::ivec2 coords = chunkRequests.front();
            chunkRequests.pop_front();

            try
            {
                // Convert grid coordinates to world coordinates
                int worldX = coords.x * Chunk::CHUNK_SIZE;
                int worldZ = coords.y * Chunk::CHUNK_SIZE;

                auto chunk = worldGen.generateChunk(root, worldX, worldZ,
                                                    Chunk::CHUNK_SIZE, Chunk::CHUNK_SIZE);

                chunks[coords] = chunk;
                chunksInGeneration++;
                // Add this: Notify when generation complete
                if (chunksInGeneration == 0)
                {
                    generationCV.notify_all();
                }
            }
            catch (...)
            {
                // Return coords to queue on failure
                chunksInGeneration--;
                chunkRequests.push_front(coords);
                throw;
            }
        }
    }

    void tickUpdate()
    {
        if (!chunkRequests.empty() && chunksInGeneration < maxConcurrentGeneration)
        {
            glm::ivec2 coords = chunkRequests.front();
            chunkRequests.pop_front();

            try
            {
                // Convert grid coordinates to world coordinates
                int worldX = coords.x * Chunk::CHUNK_SIZE;
                int worldZ = coords.y * Chunk::CHUNK_SIZE;

                auto chunk = worldGen.generateChunk(root, worldX, worldZ,
                                                    Chunk::CHUNK_SIZE, Chunk::CHUNK_SIZE);

                chunks[coords] = chunk;
                chunksInGeneration++;
                // Add this: Notify when generation complete
                if (chunksInGeneration == 0)
                {
                    generationCV.notify_all();
                }
            }
            catch (...)
            {
                // Return coords to queue on failure
                chunksInGeneration--;
                chunkRequests.push_front(coords);
                throw;
            }
        }
    }

    shared_ptr<Object> getRoot() const
    {
        return root;
    }

    // Get chunks within render distance of a point
    std::vector<std::shared_ptr<Chunk>> getChunksInRange(const glm::vec3 &center, float radius)
    {
        std::vector<std::shared_ptr<Chunk>> result;

        // Convert world position to grid coordinates
        int centerGridX = static_cast<int>(std::floor(center.x / Chunk::CHUNK_SIZE));
        int centerGridZ = static_cast<int>(std::floor(center.z / Chunk::CHUNK_SIZE));
        int gridRadius = static_cast<int>(std::ceil(radius / Chunk::CHUNK_SIZE));

        // Check chunks in square area
        for (int x = centerGridX - gridRadius; x <= centerGridX + gridRadius; x++)
        {
            for (int z = centerGridZ - gridRadius; z <= centerGridZ + gridRadius; z++)
            {
                // Direct map access instead of calling getChunk()
                auto it = chunks.find(glm::ivec2(x, z));
                if (it != chunks.end() && it->second && it->second->isReady())
                {
                    // Calculate actual distance to chunk center
                    glm::vec3 chunkCenter = it->second->getPosition() +
                                            glm::vec3(Chunk::CHUNK_SIZE / 2.0f, 0, Chunk::CHUNK_SIZE / 2.0f);
                    float distance = glm::length(center - chunkCenter);

                    if (distance <= radius)
                    {
                        result.push_back(it->second);
                    }
                }
            }
        }

        return result;
    }

private:
    shared_ptr<Object> root;
    WorldGenerator worldGen;

    // Chunk grid storage
    std::unordered_map<glm::ivec2, std::shared_ptr<Chunk>, ChunkCoordHash> chunks;
    std::deque<glm::ivec2> chunkRequests;

    // Generation control
    std::atomic<int> chunksInGeneration{0}; // Make atomic
    std::condition_variable generationCV;
    const int maxConcurrentGeneration = 4;
    bool isGenerationComplete = false;
};

#endif