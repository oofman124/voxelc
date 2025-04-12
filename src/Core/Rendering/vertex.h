#ifndef VERTEX_H
#define VERTEX_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>

struct UV_Vertex
{
    float x, y, z; // Position
    float u, v;    // Texture coordinates
};

glm::vec3 getVertexPosition(const UV_Vertex& vertex)
{
    return glm::vec3(vertex.x, vertex.y, vertex.z);
}


#endif // !VERTEX_H
