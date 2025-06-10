#ifndef BLOCK_H
#define BLOCK_H

#include <map>
#include <memory>
#include <array>
#include <glm/glm.hpp>
#include "../transform.h"
#include "../Rendering/meshRenderer.h"
#include "../object.h"
#include "../Rendering/mesh.h"
#include "../Rendering/texture.h"
#include "../atlas.h"
#include "../Util/vertex.h"


enum BlockType
{
    BLOCK_TYPE_GRASS,
    BLOCK_TYPE_DIRT,
    BLOCK_TYPE_STONE,
    BLOCK_TYPE_WOOD,
    BLOCK_TYPE_AIR,
};

enum BlockFace
{
    BLOCK_FACE_TOP,
    BLOCK_FACE_BOTTOM,
    BLOCK_FACE_LEFT,
    BLOCK_FACE_RIGHT,
    BLOCK_FACE_FRONT,
    BLOCK_FACE_BACK
};



struct BlockInfo
{
    BlockType type;
    std::string name;
    std::shared_ptr<UV_Mesh> mesh;

    // Add default constructor
    BlockInfo() 
        : type(BLOCK_TYPE_AIR), name("Air"), mesh(nullptr) {}

    // Main constructor
    BlockInfo(BlockType t, const std::string& n, const UV_Mesh& m)
        : type(t), name(n), mesh(std::make_shared<UV_Mesh>(m)) {}

    // Copy constructor
    BlockInfo(const BlockInfo& other)
        : type(other.type), name(other.name)
    {
        if (other.mesh) {
            mesh = std::make_shared<UV_Mesh>(*other.mesh);
        }
    }
};


UV_Mesh generateBlockMeshFromAtlas(std::shared_ptr<TextureAtlas> atlas, std::array<glm::vec2, 6> tileCoords);




class Block: public Object
{
public:
    Block(string name = "Block", const glm::vec3 &position = glm::vec3(0.0f), BlockType type = BLOCK_TYPE_GRASS)
        : Object(name), position(position), type(type)
    {
        ClassName = "Block";
        AddAncestorClass("Block");
    }
    BlockType getType() const { return type; }
    void setType(BlockType newType) { type = newType; }

private:
    BlockType type = BLOCK_TYPE_GRASS;
    const glm::vec3 &position;
};

#endif // BLOCK_H