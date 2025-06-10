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
        glm::vec3 pos = origin;
        glm::ivec3 blockPos = glm::floor(pos);
        glm::vec3 rayDir = glm::normalize(direction);

        glm::vec3 deltaDist = glm::abs(glm::vec3(1.0f) / rayDir);
        glm::ivec3 step(
            rayDir.x > 0 ? 1 : -1,
            rayDir.y > 0 ? 1 : -1,
            rayDir.z > 0 ? 1 : -1);
        glm::vec3 sideDist;
        for (int i = 0; i < 3; ++i)
        {
            if (rayDir[i] > 0)
                sideDist[i] = (float(blockPos[i]) + 1.0f - pos[i]) * deltaDist[i];
            else
                sideDist[i] = (pos[i] - float(blockPos[i])) * deltaDist[i];
        }

        float distance = 0.0f;
        for (int i = 0; i < int(maxDistance * 3); ++i)
        {
            // Find the chunk for this block
            int chunkX = static_cast<int>(std::floor(float(blockPos.x) / Chunk::CHUNK_SIZE));
            int chunkZ = static_cast<int>(std::floor(float(blockPos.z) / Chunk::CHUNK_SIZE));
            auto chunk = getChunk(chunkX, chunkZ);
            if (chunk)
            {
                int localX = blockPos.x - chunkX * Chunk::CHUNK_SIZE;
                int localY = blockPos.y;
                int localZ = blockPos.z - chunkZ * Chunk::CHUNK_SIZE;
                BlockType type = chunk->getBlock(localX, localY, localZ);
                if (type != BLOCK_TYPE_AIR)
                {
                    // Use AABB for precise intersection and normal
                    AABB aabb = getBlockAABB(blockPos.x, blockPos.y, blockPos.z);
                    auto result = aabb.raycast(origin, rayDir);
                    if (result.hit && result.tNear >= 0 && result.tNear <= maxDistance)
                    {
                        glm::vec3 hitPoint = origin + rayDir * result.tNear;
                        hitInfo.blockPos = blockPos;
                        hitInfo.blockType = type;
                        hitInfo.distance = result.tNear;
                        hitInfo.normal = result.normal;
                        hitInfo.chunk = chunk;
                        return true;
                    }
                }
            }
            // Advance to next voxel
            if (sideDist.x < sideDist.y && sideDist.x < sideDist.z)
            {
                blockPos.x += step.x;
                distance = sideDist.x;
                sideDist.x += deltaDist.x;
            }
            else if (sideDist.y < sideDist.z)
            {
                blockPos.y += step.y;
                distance = sideDist.y;
                sideDist.y += deltaDist.y;
            }
            else
            {
                blockPos.z += step.z;
                distance = sideDist.z;
                sideDist.z += deltaDist.z;
            }
            if (distance > maxDistance)
                break;
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