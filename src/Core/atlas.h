#ifndef ATLAS_H
#define ATLAS_H

#include <vector>
#include <memory>
#include <map>
#include <array>
#include <glm/glm.hpp>
#include "Rendering/texture.h"

class TextureAtlas {
public:
    TextureAtlas(std::shared_ptr<Texture> texture, int tileWidth, int tileHeight)
        : texture(texture), tileWidth(tileWidth), tileHeight(tileHeight) 
    {
        if (!texture) throw std::runtime_error("Null texture provided to TextureAtlas");
        
        atlasWidth = texture->getWidth();
        atlasHeight = texture->getHeight();
        
        tilesX = atlasWidth / tileWidth;
        tilesY = atlasHeight / tileHeight;
        
        if (tilesX == 0 || tilesY == 0) {
            throw std::runtime_error("Tile size larger than atlas texture");
        }
    }

    ~TextureAtlas() {
        if (isBound()) unbind();
    }

    void bind() {
        if (!texture) return;
        texture->bind();
        bound = true;
    }

    void unbind() {
        if (!texture) return;
        texture->unbind();
        bound = false;
    }

    bool isBound() const {
        return bound && texture && texture->isBound();
    }

    glm::vec4 getUV(int x, int y) const {
        if (x >= tilesX || y >= tilesY) return glm::vec4(0.0f);
        
        return glm::vec4(
            (float)(x * tileWidth) / atlasWidth,
            (float)(y * tileHeight) / atlasHeight,
            (float)((x + 1) * tileWidth) / atlasWidth,
            (float)((y + 1) * tileHeight) / atlasHeight
        );
    }
    std::array<float, 8> sampleUV(int x, int y) {
        if (x >= tilesX || y >= tilesY) return {
            0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f
        };
        return {
            (float)(x * tileWidth) / atlasWidth,
            (float)(y * tileHeight) / atlasHeight,
            (float)((x + 1) * tileWidth) / atlasWidth,
            (float)((y + 1) * tileHeight) / atlasHeight,
            (float)(x * tileWidth) / atlasWidth,
            (float)((y + 1) * tileHeight) / atlasHeight,
            (float)((x + 1) * tileWidth) / atlasWidth,
            (float)(y * tileHeight) / atlasHeight
        };
    }

    int getTilesX() const { return tilesX; }
    int getTilesY() const { return tilesY; }
    int getTileWidth() const { return tileWidth; }
    int getTileHeight() const { return tileHeight; }

private:
    std::shared_ptr<Texture> texture;
    int tileWidth, tileHeight;
    int atlasWidth, atlasHeight;
    int tilesX, tilesY;
    bool bound = false;
};

#endif // ATLAS_H