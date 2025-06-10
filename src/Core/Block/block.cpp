#include "block.h"
#include <map>
#include <memory>
#include <array>
#include <glm/glm.hpp>

UV_Mesh generateBlockMeshFromAtlas(std::shared_ptr<TextureAtlas> atlas, std::array<glm::vec2, 6> tileCoords) {
    // Calculate UV coordinates for each tile
    auto getUVs = [&atlas](glm::vec2 tileCoord) {
        float tileSize = 1.0f / 16.0f; //atlas->getTilesPerRow(); // If atlas is 16x16, this is 1/16
        glm::vec2 uvMin = tileCoord * tileSize;
        glm::vec2 uvMax = uvMin + tileSize;
        return std::make_pair(uvMin, uvMax);
    };

    std::vector<Vertex> vertices;
    vertices.reserve(24); // 6 faces * 4 vertices

    // For each face, get UVs from atlas based on tile coordinates
    for (int face = 0; face < 6; face++) {
        auto [uvMin, uvMax] = getUVs(tileCoords[face]);
        
        switch(face) {
            case BLOCK_FACE_TOP: // Top face
                vertices.push_back({-0.5f,  0.5f, -0.5f, uvMin.x, uvMin.y});
                vertices.push_back({ 0.5f,  0.5f, -0.5f, uvMax.x, uvMin.y});
                vertices.push_back({ 0.5f,  0.5f,  0.5f, uvMax.x, uvMax.y});
                vertices.push_back({-0.5f,  0.5f,  0.5f, uvMin.x, uvMax.y});
                break;
            case BLOCK_FACE_BOTTOM: // Bottom face
                vertices.push_back({-0.5f, -0.5f, -0.5f, uvMin.x, uvMin.y});
                vertices.push_back({ 0.5f, -0.5f, -0.5f, uvMax.x, uvMin.y});
                vertices.push_back({ 0.5f, -0.5f,  0.5f, uvMax.x, uvMax.y});
                vertices.push_back({-0.5f, -0.5f,  0.5f, uvMin.x, uvMax.y});
                break;
            case BLOCK_FACE_FRONT: // Front face
                vertices.push_back({-0.5f, -0.5f,  0.5f, uvMin.x, uvMax.y});
                vertices.push_back({ 0.5f, -0.5f,  0.5f, uvMax.x, uvMax.y});
                vertices.push_back({ 0.5f,  0.5f,  0.5f, uvMax.x, uvMin.y});
                vertices.push_back({-0.5f,  0.5f,  0.5f, uvMin.x, uvMin.y});
                break;
            case BLOCK_FACE_BACK: // Back face
                vertices.push_back({-0.5f, -0.5f, -0.5f, uvMax.x, uvMax.y});
                vertices.push_back({ 0.5f, -0.5f, -0.5f, uvMin.x, uvMax.y});
                vertices.push_back({ 0.5f,  0.5f, -0.5f, uvMin.x, uvMin.y});
                vertices.push_back({-0.5f,  0.5f, -0.5f, uvMax.x, uvMin.y});
                break;
            case BLOCK_FACE_LEFT: // Left face
                vertices.push_back({-0.5f, -0.5f, -0.5f, uvMin.x, uvMax.y});
                vertices.push_back({-0.5f, -0.5f,  0.5f, uvMax.x, uvMax.y});
                vertices.push_back({-0.5f,  0.5f,  0.5f, uvMax.x, uvMin.y});
                vertices.push_back({-0.5f,  0.5f, -0.5f, uvMin.x, uvMin.y});
                break;
            case BLOCK_FACE_RIGHT: // Right face
                vertices.push_back({ 0.5f, -0.5f, -0.5f, uvMax.x, uvMax.y});
                vertices.push_back({ 0.5f, -0.5f,  0.5f, uvMin.x, uvMax.y});
                vertices.push_back({ 0.5f,  0.5f,  0.5f, uvMin.x, uvMin.y});
                vertices.push_back({ 0.5f,  0.5f, -0.5f, uvMax.x, uvMin.y});
                break;
        }
    }

    // Generate indices for all faces
    std::vector<unsigned int> indices;
    indices.reserve(36); // 6 faces * 6 indices
    for (int i = 0; i < 6; i++) {
        int base = i * 4;
        indices.push_back(base + 0);
        indices.push_back(base + 1);
        indices.push_back(base + 2);
        indices.push_back(base + 2);
        indices.push_back(base + 3);
        indices.push_back(base + 0);
    }

    return UV_Mesh(vertices, indices);
}