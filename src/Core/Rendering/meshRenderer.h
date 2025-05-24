#ifndef MESH_RENDERER_H
#define MESH_RENDERER_H

#define THROW_MESH_ERR true

#include <glad/glad.h>
#include <string>
#include <glm/glm.hpp>
#include "mesh.h"
#include "shader.h"
#include "texture.h"
#include "vertexBuffer.h"
#include "../transform.h"
#include "vertex.h"
#include "../Renderer/renderer.h"
#include <memory>

enum MatrixType
{
    MODEL,
    VIEW,
    PROJECTION
};
// MeshRendererMode enum to define the rendering mode of the mesh, used in the main renderer for now.
enum MeshRendererMode
{
    // Default mode for 3D rendering
    MESH_RENDERER_MODE_DEFAULT,
    // Disabled
    MESH_RENDERER_MODE_DISABLED
};

class UV_MeshRenderer : public std::enable_shared_from_this<UV_MeshRenderer>
{
public:
    // Constructor
    UV_MeshRenderer(std::shared_ptr<UV_Mesh> mesh,
                    std::shared_ptr<Shader> shader,
                    std::shared_ptr<Texture> texture,
                    std::shared_ptr<Transform> transform,
                    MeshRendererMode mode = MESH_RENDERER_MODE_DEFAULT)
        : mesh(mesh), shader(shader), texture(texture), transform(transform), mode(mode)
    {
        if (!mesh || mesh->vertices.empty() || mesh->indices.empty())
        {
            if (THROW_MESH_ERR)
                throw std::runtime_error("Mesh must have vertices and indices");
            return;
        }
        if (!shader || !transform)
        {
            if (THROW_MESH_ERR)
                throw std::runtime_error("Shader or/and Transform must be valid");
            return;
        }
        vertexBuffer = std::make_shared<UV_VertexBuffer>(mesh->vertices, mesh->indices);
        viewMatrix = glm::mat4(1.0f);
        projectionMatrix = glm::mat4(1.0f);
        isInitialized = true;
    }

    // Default constructor
    UV_MeshRenderer(std::shared_ptr<Transform> transform)
        : shader(nullptr), mesh(nullptr), texture(nullptr), vertexBuffer(nullptr),
          transform(transform), mode(MESH_RENDERER_MODE_DEFAULT)
    {
        viewMatrix = glm::mat4(1.0f);
        projectionMatrix = glm::mat4(1.0f);
    }

    // Initialize method
    bool Initialize(std::shared_ptr<Transform> transform,
                    std::shared_ptr<UV_Mesh> mesh,
                    std::shared_ptr<Shader> shader,
                    std::shared_ptr<Texture> texture,
                    MeshRendererMode mode = MESH_RENDERER_MODE_DEFAULT)
    {
        /*
        if (!mesh || mesh->vertices.empty() || mesh->indices.empty())
        {
            if (THROW_MESH_ERR)
                throw std::runtime_error("Mesh must have vertices and indices");
            return false;
        }
            */
        if (!shader || !transform)
        {
            if (THROW_MESH_ERR)
                throw std::runtime_error("Shader or/and Transform must be valid");
            return false;
        }
        this->mode = mode;
        this->mesh = mesh;
        this->shader = shader;
        this->texture = texture;
        this->transform = transform;
        if (mesh)
            this->vertexBuffer = std::make_shared<UV_VertexBuffer>(mesh->vertices, mesh->indices);
        isInitialized = true;
        return true;
    }
    bool getReady() const
    {
        return isInitialized;
    }

    ~UV_MeshRenderer() = default;

    void queueToRender(std::shared_ptr<Renderer> renderer)
    {
        if (!isInitialized || !vertexBuffer)
            return;
        renderer->renderMesh(vertexBuffer, texture, transform);
    }

    void render()
    {
        if (!isInitialized || !mesh || !texture || !vertexBuffer)
            return;

        shader->use();
        shader->setMat4("model", transform->getMatrix());
        shader->setMat4("view", viewMatrix);
        shader->setMat4("projection", projectionMatrix);

        texture->bindToShaderInt(*shader, "texture0");

        vertexBuffer->bind();
        glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
        vertexBuffer->unbind();

        texture->unbind(); // DON'T forget this
    }

    void setMatrix(MatrixType type, const glm::mat4 &matrix)
    {
        switch (type)
        {
        case MODEL:
            transform->setMatrix(matrix);
            break;
        case VIEW:
            viewMatrix = matrix;
            break;
        case PROJECTION:
            projectionMatrix = matrix;
            break;
        default:
            break;
        }
    }

    void getMatrix(MatrixType type, glm::mat4 &matrix)
    {
        switch (type)
        {
        case MODEL:
            matrix = transform->getMatrix();
            break;
        case VIEW:
            matrix = viewMatrix;
            break;
        case PROJECTION:
            matrix = projectionMatrix;
            break;
        default:
            break;
        }
    }
    void setMode(MeshRendererMode mode) { this->mode = mode; }
    MeshRendererMode getMode() const { return mode; }

    std::vector<glm::vec3> getTransformedVertices()
    {
        if (!isInitialized)
            return {};
        std::vector<glm::vec3> transformedVertices;
        glm::mat4 modelMatrix = transform->getMatrix();

        for (const auto &vertex : mesh->vertices)
        {
            // Assuming vertex.position is a glm::vec3
            glm::vec4 transformed = modelMatrix * glm::vec4(glm::vec3(vertex.x, vertex.y, vertex.z), 1.0f);
            transformedVertices.push_back(glm::vec3(transformed));
        }

        return transformedVertices;
    }
    void setShader(std::shared_ptr<Shader> shader) { this->shader = shader; }
    std::shared_ptr<Shader> getShader() { return shader; }

    void setTexture(std::shared_ptr<Texture> newTexture) { texture = newTexture; }
    std::shared_ptr<Texture> getTexture() { return texture; }

    void setMesh(std::shared_ptr<UV_Mesh> newMesh)
    {
        if (isInitialized)
        {
            mesh = newMesh;
            if (vertexBuffer)
            {
                vertexBuffer->updateVertices(newMesh->vertices);
                vertexBuffer->updateIndices(newMesh->indices);
            }
            else
            {
                vertexBuffer = std::make_shared<UV_VertexBuffer>(newMesh->vertices, newMesh->indices);
            }
        }
    }

    std::shared_ptr<UV_Mesh> getMesh() { return mesh; }

    void setTransform(std::shared_ptr<Transform> transform) { this->transform = transform; }
    std::shared_ptr<Transform> getTransform() { return transform; }

    std::shared_ptr<UV_VertexBuffer> getVertexBuffer() { return vertexBuffer; }

private:
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;

    bool isInitialized = false;
    MeshRendererMode mode;
    std::shared_ptr<Transform> transform;
    std::shared_ptr<UV_Mesh> mesh;
    std::shared_ptr<Texture> texture;
    std::shared_ptr<UV_VertexBuffer> vertexBuffer;
    std::shared_ptr<Shader> shader;
};

#endif // MESH_RENDERER_H