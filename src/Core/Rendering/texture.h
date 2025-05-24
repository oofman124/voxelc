#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <stb_image.h>
#include <iostream>
#include <string>
#include <filesystem>
#include <set>
#include "shader.h"
#include <memory>

#define CRISP_TEXTURES true
#define ENABLE_FALLBACK_TEXTURE true
#define FALLBACK_TEXTURE_PATH "resources/textures/tex-placeholder.jpg"

class Texture : public std::enable_shared_from_this<Texture>
{
public:
    // Constructor that loads a texture from a file
    Texture(const std::string &path)
    {
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        // Set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // Set texture filtering parameters based on pixelated flag
        if (CRISP_TEXTURES) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        } else {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }

        // Load image, create texture and generate mipmaps
        stbi_set_flip_vertically_on_load(false);
        unsigned char *data = nullptr;

        if (std::filesystem::exists(path.c_str()))
        {
            data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
        }
        else if (ENABLE_FALLBACK_TEXTURE)
        {
            std::cerr << "Failed to load texture, using fallback: " << path << std::endl;

            // Create a separate texture object for the fallback texture
            unsigned int fallbackTextureID;
            glGenTextures(1, &fallbackTextureID);
            glBindTexture(GL_TEXTURE_2D, fallbackTextureID);

            data = stbi_load(FALLBACK_TEXTURE_PATH, &width, &height, &nrChannels, 0);

            // Assign the fallback texture ID to this instance
            textureID = fallbackTextureID;
        }

        if (data)
        {
            GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
            buffer.assign(data, data + (width * height * nrChannels));
        }
        else
        {
            std::cerr << "Failed to load texture, and fallback couldn't load: " << path << std::endl;
        }

        stbi_image_free(data);
    }

    // Destructor to clean up the texture
    ~Texture()
    {
        if (isBound())
            unbind();
        glDeleteTextures(1, &textureID);
    }
    bool isBound() const
    {
        GLint currentTexture;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);
        return currentTexture == textureID;
    }

    // Bind the texture with automatic unit selection
    void bind() const {
        unit = getNextAvailableUnit();
        glActiveTexture(unit);
        glBindTexture(GL_TEXTURE_2D, textureID);
    }

    // Bind to shader with automatic unit handling
    void bindToShaderInt(Shader &shader, const std::string &name) const {
        shader.use();
        if (!isBound())
            bind();
        int value = unit - GL_TEXTURE0;
        shader.setInt(name, value);
    }
    GLenum getUnit() const { return unit; }

    bool operator==(const Texture& other) const {
        return textureID == other.textureID;
    }

    // Unbind the texture
    void unbind() const {
        if (!isBound()) // Prevent messing with the outsider textures
            return;
        glBindTexture(GL_TEXTURE_2D, 0);
        if (unit != GL_TEXTURE0) {
            releaseUnit(unit);
            unit = GL_TEXTURE0;
        }
    }
    std::vector<unsigned char> getBuffer() const {
        return buffer;
    }
    // Get the width and height of the texture
    int getWidth() const { return width; }
    int getHeight() const { return height; }

    // Get the texture ID
    unsigned int getID() const { return textureID; }

private:
    unsigned int textureID;
    mutable GLenum unit = GL_TEXTURE0;  // Mutable since it changes in const methods
    mutable int width, height, nrChannels;
    std::vector<unsigned char> buffer;

    static GLenum getNextAvailableUnit() {
        static std::set<GLenum> usedUnits;
        
        // Find first available unit
        for (GLenum unit = GL_TEXTURE0; unit <= GL_TEXTURE31; unit++) {
            if (usedUnits.find(unit) == usedUnits.end()) {
                usedUnits.insert(unit);
                return unit;
            }
        }
        // If no units available, reuse GL_TEXTURE0
        return GL_TEXTURE0;
    }

    static void releaseUnit(GLenum unit) {
        static std::set<GLenum> usedUnits;
        usedUnits.erase(unit);
    }
};
#endif // TEXTURE_H
