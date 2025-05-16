#ifndef ASSETS_H
#define ASSETS_H
#include <unordered_map>
#include <string>
#include <memory>
#include "Rendering/shader.h"
#include "Rendering/texture.h"



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
    std::shared_ptr<Texture> addTexture(const std::string& name, const std::string& path) {
        if (textures.find(name) == textures.end()) {
            std::shared_ptr<Texture> tex = std::make_shared<Texture>(path);
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

    // Initialize default assets
    void initializeDefaultAssets() {
        // Add your default shaders
        addShader("default", "resources/shaders/vertex_texture.glsl", "resources/shaders/fragment_texture.glsl");
        addShader("ui", "resources/shaders/ui_vertex.glsl", "resources/shaders/ui_fragment.glsl");
        
        // Add your default textures
        addTexture("grass", "resources/textures/grass.png");
        addTexture("block", "resources/textures/block_sample.png");
    }

private:
    AssetManager() {} // Private constructor for singleton
    AssetManager(const AssetManager&) = delete;
    AssetManager& operator=(const AssetManager&) = delete;

    std::unordered_map<std::string, std::shared_ptr<Shader>> shaders;
    std::unordered_map<std::string, std::shared_ptr<Texture>> textures;
};

#endif