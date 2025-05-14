#ifndef UI_RENDERER_H
#define UI_RENDERER_H

#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "uiElement.h"
#include "../Rendering/shader.h"
#include "uiVertexBuffer.h" // Ensure UIVertexBuffer is included
#include "../Rendering/texture.h"
#include "../assets.h"

GLenum glCheckError_(const char *file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
        case GL_INVALID_ENUM:
            error = "INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            error = "INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            error = "INVALID_OPERATION";
            break;
        case GL_STACK_OVERFLOW:
            error = "STACK_OVERFLOW";
            break;
        case GL_STACK_UNDERFLOW:
            error = "STACK_UNDERFLOW";
            break;
        case GL_OUT_OF_MEMORY:
            error = "OUT_OF_MEMORY";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            error = "INVALID_FRAMEBUFFER_OPERATION";
            break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

class UIRenderer
{
private:
    struct UIBatch
    {
        std::vector<UIVertex> vertices;
        std::vector<uint32_t> indices;
        std::shared_ptr<Texture> texture;
    };

    std::vector<UIBatch> batches;
    std::unique_ptr<Shader> uiShader;
    std::unique_ptr<UIVertexBuffer> vertexBuffer = std::make_unique<UIVertexBuffer>();
    glm::mat4 orthoProjection;

public:
    UIRenderer(int width, int height)
    {
        // Create orthographic projection for UI
        orthoProjection = glm::ortho(0.0f, (float)width, (float)height, 0.0f, -1.0f, 1.0f);

        // Initialize UI shader
        uiShader = std::make_unique<Shader>(*assetMgr.getShader("ui")); // Convert shared_ptr to unique_ptr
    }

    void SetProjection(int width, int height)
    {
        orthoProjection = glm::ortho(0.0f, (float)width, 0.0f, (float)height, -1.0f, 1.0f);
        if (uiShader)
        {
            uiShader->use();
            uiShader->setMat4("projection", orthoProjection);
        }
    }

    void begin()
    {

        batches.clear();
    }

    void submit(UIElement *element /*, std::shared_ptr<Texture> texture = nullptr */)
    {
        // Find or create appropriate batch
        UIBatch *targetBatch = nullptr;
        for (auto &batch : batches)
        {
            if ((batch.texture.get() == element->getTexture().get()) || (!batch.texture && !element->getTexture()))
            {
                targetBatch = &batch;
                break;
            }
        }

        if (!targetBatch)
        {
            batches.push_back(UIBatch());
            targetBatch = &batches.back();
        }

        // Add vertices and indices
        uint32_t indexOffset = targetBatch->vertices.size();
        auto &elementVerts = element->getVertices();
        auto &elementIndices = element->getIndices();

        targetBatch->vertices.insert(targetBatch->vertices.end(),
                                     elementVerts.begin(),
                                     elementVerts.end());
        targetBatch->texture = element->getTexture() ? std::make_shared<Texture>(*element->getTexture()) : nullptr;

        for (auto index : elementIndices)
        {
            targetBatch->indices.push_back(index + indexOffset);
        }
    }

    void flush()
    {
        if (batches.empty())
            return;

        glEnable(GL_BLEND); // Enable this for UI transparency
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        uiShader->use();
        uiShader->setMat4("projection", orthoProjection);
        glCheckError();

        // Debug output
        GLint program;
        glGetIntegerv(GL_CURRENT_PROGRAM, &program);
        std::cout << "Current shader program: " << program << std::endl;

        GLint maxAttribs;
        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxAttribs);
        std::cout << "Max vertex attributes: " << maxAttribs << std::endl;

        for (const auto &batch : batches)
        {
            if (batch.vertices.empty())
                continue;

            // Bind VAO first
            vertexBuffer->bind();

            // Update buffer data with proper size checks
            if (!batch.vertices.empty() && !batch.indices.empty())
            {
                vertexBuffer->updateData(batch.vertices, batch.indices);
            }

            // Handle texture binding
            if (batch.texture && batch.texture->getID() != 0)
            {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, batch.texture->getID());
                uiShader->setInt("hasTexture", true);
                uiShader->setInt("uiTexture", 0);
            }
            else
            {
                uiShader->setInt("hasTexture", false);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, 0);
            }

            // Verify attribute states
            GLint enabled;
            for (int i = 0; i < 3; i++)
            {
                glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &enabled);
                std::cout << "Attribute " << i << " enabled: " << enabled << std::endl;
            }

            // Draw
            glDrawElements(GL_TRIANGLES, batch.indices.size(), GL_UNSIGNED_INT, 0);
            glCheckError();

            // Cleanup
            if (batch.texture)
            {
                glBindTexture(GL_TEXTURE_2D, 0);
            }
            vertexBuffer->unbind();
        }

        glDisable(GL_BLEND);
        batches.clear();
    }
};

#endif