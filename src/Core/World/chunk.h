#ifndef CHUNK_H
#define CHUNK_H

#include <iostream>
#include <vector>
#include <memory>
#include <mutex>
#include <atomic>
#include "../transform.h"
#include "../Rendering/meshRenderer.h"
#include "../Renderer/renderer.h"
#include "../object.h"
#include "../assets.h"
#include "../Block/block.h"
#include "../Util/vertex.h"
#include <array>

enum class ChunkState
{
    UNLOADED,
    GENERATING,
    READY
};
enum class ChunkMeshState
{
    OUTDATED,
    GENERATING,
    QUEUED,
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
        if (!isValidPosition(x, y, z))
            return;

        int index = getIndex(x, y, z);

        if (type == BLOCK_TYPE_AIR)
        {
            // Remove the block if it exists
            blocks[index] = nullptr;
        }
        else
        {
            if (!blocks[index])
            {
                blocks[index] = std::make_shared<Block>("Block",
                                                        glm::vec3(x, y, z), type);
            }
            else
            {
                blocks[index]->setType(type);
            }
        }

        meshState.store(ChunkMeshState::OUTDATED);
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
        if (meshState.load() != ChunkMeshState::OUTDATED)
            return;
        meshState.store(ChunkMeshState::GENERATING);
        std::vector<Vertex> vertices;
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
                        Vertex v = vertex;
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

        if (vertices.empty() || indices.empty()) {
            std::cout << "Warning: Generated empty mesh for chunk at " << position.x << "," << position.y << "," << position.z << std::endl;
        }
        // Create or update the mesh
        mesh = std::make_shared<UV_Mesh>(vertices, indices);
        // meshRenderer->setMesh(mesh); calls opengl functions; only call on renderer thread ):<
        meshState.store(ChunkMeshState::QUEUED);
        setState(ChunkState::READY);
    }

    void queueToRenderer(std::shared_ptr<Renderer> renderer)
    {
        if (!isReady())
            return;

        if (meshState.load() == ChunkMeshState::OUTDATED)
        {
            updateMesh();
            if (mesh)
            {
                meshRenderer->setMesh(mesh);
                meshState.store(ChunkMeshState::READY);
            }
        } else if (meshState.load() == ChunkMeshState::QUEUED && mesh)
        {
            meshRenderer->setMesh(mesh);
            meshState.store(ChunkMeshState::READY);
        }
        if (meshRenderer && meshState.load() == ChunkMeshState::READY)
        {
            meshRenderer->queueToRender(renderer);
        }
    }
    bool isReady() const
    {
        return state.load() == ChunkState::READY;
    }



    void setState(ChunkState newState)
    {
        state.store(newState);
    }

    void setPosition(const glm::vec3 &pos)
    {
        position = pos;
        if (transform)
        {
            transform->setPosition(pos);
        }
    }

    glm::vec3 getPosition() const
    {
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
    std::shared_ptr<UV_Mesh> mesh;
    std::atomic<ChunkState> state{ChunkState::UNLOADED};
    std::atomic<ChunkMeshState> meshState{ChunkMeshState::OUTDATED};
    std::vector<std::shared_ptr<Block>> blocks;
    std::shared_ptr<UV_MeshRenderer> meshRenderer;
    std::shared_ptr<Transform> transform;
    glm::vec3 position{0.0f};
};

#endif