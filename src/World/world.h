#ifndef WORLD_H
#define WORLD_H

#include <memory>
#include "worldGenerator.h"
#include "../Core/instance.h"

class World {
public:
    World() 
        : worldGen() {
        root = make_shared<Instance>("World");
    }

    void generateTerrain(int width = 10, int depth = 10) {
        // Center the terrain around origin
        int startX = -width / 2;
        int startZ = -depth / 2;
        worldGen.generateChunk(root, startX, startZ, width, depth);
    }

    shared_ptr<Instance> getRoot() const { return root; }

private:
    shared_ptr<Instance> root;
    WorldGenerator worldGen;
};

#endif