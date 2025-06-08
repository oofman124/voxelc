#ifndef VERTEX2D_H
#define VERTEX2D_H

#include <glm/gtc/matrix_transform.hpp>

struct Vertex2D {
    glm::vec2 pos;
    glm::vec2 uv;
    glm::vec4 color;
};
#endif // VERTEX2D_H