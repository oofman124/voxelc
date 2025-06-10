#ifndef VERTEX_H
#define VERTEX_H

#include <glm/gtc/matrix_transform.hpp>


struct Vertex2D {
    glm::vec2 pos;
    glm::vec2 uv;
    glm::vec4 color;
};

struct Vertex {
    float x, y, z; // Position
    float u, v;    // Texture coordinates
};

// For AABB mesh visualization or debug drawing
struct SpatialVertex {
    glm::vec3 pos;
    glm::vec3 normal;
};















#endif // !VERTEX_H