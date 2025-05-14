#ifndef UIVertex_H
#define UIVertex_H

struct UIVertex {
    float x, y;       // Screen space coordinates
    float u, v;       // Texture coordinates
    float r, g, b, a; // RGBA color
};

#endif