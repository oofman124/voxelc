#ifndef WORLD_H
#define WORLD_H

#include <memory>
#include <vector>
#include <unordered_map>
#include <queue>
#include "worldGenerator.h"
#include "../object.h"
#include "../Util/AABB.h"

// Chunk grid coordinate hasher
struct ChunkCoordHash
{
    std::size_t operator()(const glm::ivec2 &k) const
    {
        return std::hash<int>()(k.x) ^ (std::hash<int>()(k.y) << 1);
    }
};

struct BlockRaycastHit
{
    glm::ivec3 blockPos;
    BlockType blockType;
    float distance;
    glm::vec3 normal;
    std::shared_ptr<Chunk> chunk;
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

    inline AABB getBlockAABB(int x, int y, int z)
    {
        glm::vec3 min(x, y, z);
        glm::vec3 max = min + glm::vec3(1.0f);
        return {min, max};
    }

    bool raycast(const glm::vec3 &origin, const glm::vec3 &direction, float maxDistance, BlockRaycastHit &hitInfo)
    {
        glm::vec3 rayDir = glm::normalize(direction);
        glm::vec3 rayOrigin = origin;

        // Find starting chunk
        glm::ivec2 chunkCoord(
            static_cast<int>(std::floor(rayOrigin.x / Chunk::CHUNK_SIZE)),
            static_cast<int>(std::floor(rayOrigin.z / Chunk::CHUNK_SIZE)));

        float closestT = maxDistance;
        bool found = false;
        SpatialRaycastResult bestResult;
        std::shared_ptr<Chunk> bestChunk = nullptr;

        // Traverse chunks along the ray
        // Limit: up to maxDistance/chunkSize + 2 chunks (for safety)
        int maxChunks = static_cast<int>(maxDistance / Chunk::CHUNK_SIZE) + 2;
        glm::vec3 pos = rayOrigin;
        float traveled = 0.0f;

        for (int i = 0; i < maxChunks && traveled < maxDistance; ++i)
        {
            // Get chunk at current position
            glm::ivec2 currChunkCoord(
                static_cast<int>(std::floor(pos.x / Chunk::CHUNK_SIZE)),
                static_cast<int>(std::floor(pos.z / Chunk::CHUNK_SIZE)));
            auto chunk = getChunk(currChunkCoord.x, currChunkCoord.y);
            if (chunk && chunk->isReady())
            {
                auto spatialMesh = chunk->getSpatialMesh();
                if (spatialMesh)
                {
                    SpatialRaycastResult result;
                    if (spatialMesh->raycastWorld(rayOrigin, rayDir, result) && result.t >= 0 && result.t < closestT)
                    {
                        closestT = result.t;
                        bestResult = result;
                        bestChunk = chunk;
                        found = true;
                    }
                }
            }

            // Step to next chunk boundary along the ray
            glm::vec3 chunkMin = glm::vec3(currChunkCoord.x * Chunk::CHUNK_SIZE, -FLT_MAX, currChunkCoord.y * Chunk::CHUNK_SIZE);
            glm::vec3 chunkMax = chunkMin + glm::vec3(Chunk::CHUNK_SIZE, FLT_MAX, Chunk::CHUNK_SIZE);

            // Ray-AABB intersection to find exit point
            AABB chunkAABB{chunkMin, chunkMax};
            auto aabbResult = chunkAABB.raycast(pos, rayDir);
            if (!aabbResult.hit || aabbResult.tFar <= 0)
                break; // Ray leaves world

            float stepDist = std::max(aabbResult.tFar, 0.01f);
            pos = pos + rayDir * (stepDist + 0.001f); // Step just past the boundary
            traveled = glm::length(pos - rayOrigin);
        }

        if (found)
        {
            glm::vec3 hitPoint = rayOrigin + rayDir * bestResult.t;
            glm::ivec3 hitBlock = glm::floor(hitPoint);

            hitInfo.blockPos = hitBlock;
            int chunkX = static_cast<int>(std::floor(float(hitBlock.x) / Chunk::CHUNK_SIZE));
            int chunkZ = static_cast<int>(std::floor(float(hitBlock.z) / Chunk::CHUNK_SIZE));
            auto chunk = getChunk(chunkX, chunkZ);
            if (chunk)
            {
                hitInfo.blockType = chunk->getBlock(
                    hitBlock.x - chunkX * Chunk::CHUNK_SIZE,
                    hitBlock.y,
                    hitBlock.z - chunkZ * Chunk::CHUNK_SIZE);
            }
            else
            {
                hitInfo.blockType = BLOCK_TYPE_AIR;
            }
            hitInfo.distance = bestResult.t;
            hitInfo.normal = bestResult.normal;
            hitInfo.chunk = chunk;
            return true;
        }
        return false;
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