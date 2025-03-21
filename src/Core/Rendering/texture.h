#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>
#include <string>
#include "shader.h"

class Texture
{
public:
    // Constructor that loads a texture from a file
    Texture(const std::string& path, GLenum textureUnit)
        : unit(textureUnit)
    {
        glGenTextures(1, &textureID);
        glActiveTexture(textureUnit);
        glBindTexture(GL_TEXTURE_2D, textureID);

        // Set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // Set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Load image, create texture and generate mipmaps
        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            /*
                GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);
            */
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cerr << "Failed to load texture: " << path << std::endl;
        }
        stbi_image_free(data);
    }

    // Destructor to clean up the texture
    ~Texture()
    {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteTextures(1, &textureID);
    }

    // Bind the texture
    void bind() const
    {
        glActiveTexture(unit);
        glBindTexture(GL_TEXTURE_2D, textureID);
    }
    // Bind to shader
	void bindToShaderInt(Shader& shader, const std::string& name) const
	{
        shader.use();
        bind();
		int value = unit - GL_TEXTURE0;
		// std::cout << "Texture unit: " << value << std::endl;
		shader.setInt(name, value);
	}
    GLenum getUnit() const
    {
        return unit;
    }


    // Unbind the texture
    void unbind() const
    {
        glBindTexture(GL_TEXTURE_2D, textureID);
    }

    // Get the texture ID
    unsigned int getID() const
    {
        return textureID;
    }

private:
    unsigned int textureID;
    GLenum unit;
};

#endif // TEXTURE_H

