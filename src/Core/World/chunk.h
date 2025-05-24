#ifndef CHUNK_H
#define CHUNK_H

#include <vector>
#include <memory>
#include <mutex>
#include <atomic>
#include "../transform.h"
#include "../Rendering/meshRenderer.h"
#include "../Renderer/renderer.h"
#include "../object.h"
#include "../assets.h"
#include "../block.h"
#include <array>

enum class ChunkState
{
    UNLOADED,
    GENERATING,
    READY
};

class Chunk : public Object
{
public:
    static const int CHUNK_SIZE = 16;
    static const int CHUNK_HEIGHT = 256;

    Chunk(const std::string &name = "Chunk") : Object(name)
    {
        ClassName = "Chunk";
        AddAncestorClass("Chunk");

        // Initialize the blocks array
        blocks.resize(CHUNK_SIZE * CHUNK_SIZE * CHUNK_HEIGHT, nullptr);

        // Create mesh renderer with transform
        transform = std::make_shared<Transform>();
        meshRenderer = std::make_shared<UV_MeshRenderer>(transform);

        // Get the terrain texture from asset manager
        auto &assets = AssetManager::getInstance();
        auto texture = assets.getTexture("terrain");
        auto shader = assets.getShader("default");

        if (!texture || !shader)
        {
            throw std::runtime_error("Required assets not found");
        }
        meshRenderer->Initialize(transform, nullptr, shader, texture);
    }


    void setBlock(int x, int y, int z, BlockType type)
    {
        std::lock_guard<std::mutex> lock(chunkMutex); // RAII lock
        if (!isValidPosition(x, y, z))
            return;

        int index = getIndex(x, y, z);
        if (!blocks[index])
        {
            blocks[index] = std::make_shared<Block>("Block",
                                                    glm::vec3(x, y, z), type);
        }
        else
        {
            blocks[index]->setType(type);
        }

        needsMeshUpdate.store(true);
    }

    BlockType getBlock(int x, int y, int z) const
    {
        if (!isValidPosition(x, y, z))
            return BLOCK_TYPE_AIR;

        int index = getIndex(x, y, z);
        return blocks[index] ? blocks[index]->getType() : BLOCK_TYPE_AIR;
    }

    void updateMesh()
    {
        std::lock_guard<std::mutex> lock(chunkMutex);
        if (!needsMeshUpdate.load())
            return;

        std::vector<UV_Vertex> vertices;
        std::vector<unsigned int> indices;

        // For each block in the chunk
        for (int x = 0; x < CHUNK_SIZE; x++)
        {
            for (int y = 0; y < CHUNK_HEIGHT; y++)
            {
                for (int z = 0; z < CHUNK_SIZE; z++)
                {
                    auto block = getBlock(x, y, z);
                    if (block == BLOCK_TYPE_AIR)
                        continue;

                    // Get block info from database
                    const auto &blockInfo = BlockDatabase::getBlockInfo(block);

                    // Add block mesh vertices with offset
                    auto &blockMesh = blockInfo.mesh;
                    size_t baseIndex = vertices.size();

                    for (const auto &vertex : blockMesh->vertices)
                    {
                        UV_Vertex v = vertex;
                        v.x += x;
                        v.y += y;
                        v.z += z;
                        vertices.push_back(v);
                    }

                    // Add indices with offset
                    for (auto index : blockMesh->indices)
                    {
                        indices.push_back(index + baseIndex);
                    }
                }
            }
        }

        // Create or update the mesh
        auto mesh = std::make_shared<UV_Mesh>(vertices, indices);
        meshRenderer->setMesh(mesh);

        needsMeshUpdate.store(false);
    }

    void queueToRenderer(std::shared_ptr<Renderer> renderer)
    {
        std::lock_guard<std::mutex> lock(chunkMutex);
        if (!isReady())
            return;

        if (needsMeshUpdate.load())
        {
            updateMesh();
        }
        if (meshRenderer)
        {
            meshRenderer->queueToRender(renderer);
        }
    }
    bool isReady() const
    {
        return state.load() == ChunkState::READY;
    }

    void lock()
    {
        chunkMutex.lock();
    }

    void unlock()
    {
        chunkMutex.unlock();
    }

    void setState(ChunkState newState)
    {
        state.store(newState);
    }

    void setPosition(const glm::vec3 &pos)
    {
        std::lock_guard<std::mutex> lock(chunkMutex); // Add lock
        position = pos;
        if (transform)
        {
            transform->setPosition(pos);
        }
    }

    glm::vec3 getPosition() const
    {
        std::lock_guard<std::mutex> lock(chunkMutex); // Add lock
        return position;
    }

private:
    bool isValidPosition(int x, int y, int z) const
    {
        return x >= 0 && x < CHUNK_SIZE &&
               y >= 0 && y < CHUNK_HEIGHT &&
               z >= 0 && z < CHUNK_SIZE;
    }

    int getIndex(int x, int y, int z) const
    {
        return (y * CHUNK_SIZE * CHUNK_SIZE) + (z * CHUNK_SIZE) + x;
    }

    mutable std::mutex chunkMutex;
    std::atomic<ChunkState> state{ChunkState::UNLOADED};
    std::vector<std::shared_ptr<Block>> blocks;
    std::shared_ptr<UV_MeshRenderer> meshRenderer;
    std::shared_ptr<Transform> transform;
    std::atomic<bool> needsMeshUpdate{true}; // Make atomic
    glm::vec3 position{0.0f};
};

#endif