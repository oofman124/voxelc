#ifndef CHUNK_H
#define CHUNK_H

#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include "../Core/transform.h"
#include "../Core/Rendering/meshRenderer.h"
#include "../Core/object.h"

class Chunk : public Object
{
public:

    Chunk(string name = "Chunk")
        : Object(name)
    {
        ClassName = "Chunk";
        AddAncestorClass("Chunk");
    }
    glm::vec3 getPosition() const
    {
        return position;
    }
    void setPosition(const glm::vec3 &pos)
    {
        position = pos;
    }
private:
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
};

#endif