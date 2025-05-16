#ifndef WORLD_H
#define WORLD_H

#include <memory>
#include "worldGenerator.h"
#include "../Core/object.h"

class World
{
public:
    World()
        : worldGen()
    {
        root = make_shared<Object>("World");
    }

    void generateTerrain(int width = 10, int depth = 10)
    {
        // width and depth are now in chunks
        int chunkSize = 16;
        int totalWidth = width * chunkSize;
        int totalDepth = depth * chunkSize;
        int startX = -totalWidth / 2;
        int startZ = -totalDepth / 2;

        for (int chunkX = 0; chunkX < width; ++chunkX)
        {
            for (int chunkZ = 0; chunkZ < depth; ++chunkZ)
            {
                int chunkOriginX = startX + chunkX * chunkSize;
                int chunkOriginZ = startZ + chunkZ * chunkSize;
                worldGen.generateChunk(root, chunkOriginX, chunkOriginZ, chunkSize, chunkSize);
            }
        }
    }
    shared_ptr<Object> getRoot() const { return root; }

private:
    shared_ptr<Object> root;
    WorldGenerator worldGen;
};

#endif