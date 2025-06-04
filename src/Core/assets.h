#ifndef ASSETS_H
#define ASSETS_H
#include <unordered_map>
#include <string>
#include <memory>
#include "Rendering/shader.h"
#include "Rendering/texture.h"
#include "Block/blockDatabase.h"
#include "atlas.h"


class AssetManager {
public:
    static AssetManager& getInstance() {
        static AssetManager instance;
        return instance;
    }

    // Shader management
    void addShader(const std::string& name, const char* vertexPath, const char* fragmentPath) {
        if (shaders.find(name) == shaders.end()) {
            shaders[name] = std::make_shared<Shader>(vertexPath, fragmentPath);
        }
    }

    std::shared_ptr<Shader> getShader(const std::string& name) {
        auto it = shaders.find(name);
        if (it != shaders.end()) {
            return it->second;
        }
        return nullptr;
    }

    // Texture management
    std::shared_ptr<Texture> addTexture(const std::string& name, const std::string& path, bool flip = false) {
        if (textures.find(name) == textures.end()) {
            std::shared_ptr<Texture> tex = std::make_shared<Texture>(path, flip);
            textures[name] = tex;
            return tex;
        }
        return nullptr; // Texture already exists
    }

    std::shared_ptr<Texture> getTexture(const std::string& name) {
        auto it = textures.find(name);
        if (it != textures.end()) {
            return it->second;
        }
        return nullptr;
    }

    // Atlas management
    std::shared_ptr<TextureAtlas> addTextureAtlas(const std::string& name, const std::shared_ptr<Texture>& texture, int tileWidth, int tileHeight) {
        if (textureAtlases.find(name) == textureAtlases.end()) {
            auto atlas = std::make_shared<TextureAtlas>(texture, tileWidth, tileHeight);
            textureAtlases[name] = atlas;
            return atlas;
        }
        return nullptr; // Atlas already exists
    }
    std::shared_ptr<TextureAtlas> getTextureAtlas(const std::string& name) {
        auto it = textureAtlases.find(name);
        if (it != textureAtlases.end()) {
            return it->second;
        }
        return nullptr;
    }

    // Initialize default assets
    void initializeDefaultAssets() {
        // Add your default shaders
        addShader("default", "resources/shaders/vertex_texture.glsl", "resources/shaders/fragment_texture.glsl");
        addShader("ui", "resources/shaders/vertex_2d.glsl", "resources/shaders/fragment_2d.glsl");
        
        // Add your default textures
        addTexture("grass", "resources/textures/grass.png");
        addTexture("block", "resources/textures/block_sample.png");
        addTexture("terrain", "resources/textures/terrain.png");
        addTexture("placeholder", "resources/textures/tex-placeholder.png", true);
        addTexture("notch", "resources/textures/notch.jpg", true);

        // Add your default texture atlases
        if (getTexture("terrain")) {
            if (addTextureAtlas("terrain_atlas", getTexture("terrain"), 16, 16)) {
                BlockDatabase::initialize(getTextureAtlas("terrain_atlas"));
            }
        }
    }

private:
    AssetManager() {} // Private constructor for singleton
    AssetManager(const AssetManager&) = delete;
    AssetManager& operator=(const AssetManager&) = delete;

    std::unordered_map<std::string, std::shared_ptr<Shader>> shaders;
    std::unordered_map<std::string, std::shared_ptr<Texture>> textures;
    std::unordered_map<std::string, std::shared_ptr<TextureAtlas>> textureAtlases;
};

#endif