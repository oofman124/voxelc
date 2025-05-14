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
    UV_MeshRenderer(const UV_Mesh &mesh, Shader *shader, Texture* texture, Transform *transform, MeshRendererMode mode = MESH_RENDERER_MODE_DEFAULT)
        : mesh(new UV_Mesh(mesh)), shader(shader), texture(texture),
          transform(transform), mode(mode)
    {
        if (mesh.vertices.empty() || mesh.indices.empty())
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

        // Initialize vertex buffer
        vertexBuffer = new UV_VertexBuffer(mesh.vertices, mesh.indices);

        // Initialize matrices
        viewMatrix = glm::mat4(1.0f);
        projectionMatrix = glm::mat4(1.0f);
        isInitialized = true;
    }

    // Default constructor
    UV_MeshRenderer(Transform *transform)
        : shader(nullptr), mesh(nullptr), texture(nullptr), vertexBuffer(nullptr),
          transform(transform), mode(MESH_RENDERER_MODE_DEFAULT)
    {
        // Initialize matrices
        viewMatrix = glm::mat4(1.0f);
        projectionMatrix = glm::mat4(1.0f);
    }
    // Initialize method for setting up after default construction
    bool Initialize(Transform *transform, const UV_Mesh &mesh, Shader *shader, Texture* texture, MeshRendererMode mode = MESH_RENDERER_MODE_DEFAULT)
    {
        if (mesh.vertices.empty() || mesh.indices.empty())
        {
            if (THROW_MESH_ERR)
                throw std::runtime_error("Mesh must have vertices and indices");
            return false;
        }
        if (!shader || !transform)
        {
            if (THROW_MESH_ERR)
                throw std::runtime_error("Shader or/and Transform must be valid");
            return false;
        }

        this->mode = mode;
        this->mesh = new UV_Mesh(mesh);
        this->shader = shader;
        this->texture = texture;
        this->transform = transform;
        this->vertexBuffer = new UV_VertexBuffer(mesh.vertices, mesh.indices);
        isInitialized = true;
        return true;
    }

    bool getReady() const
    {
        return isInitialized;
    }

    ~UV_MeshRenderer()
    {
        // Clean up
        delete vertexBuffer;
        delete mesh;
    }

    void render() {
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

    std::vector<glm::vec3> getTransformedVertices()
    {
        if (!isInitialized)
            return {};
        std::vector<glm::vec3> transformedVertices;
        glm::mat4 modelMatrix = transform->getMatrix();

        for (const auto &vertex : mesh->vertices)
        {
            // Assuming vertex.position is a glm::vec3
            glm::vec4 transformed = modelMatrix * glm::vec4(getVertexPosition(vertex), 1.0f);
            transformedVertices.push_back(glm::vec3(transformed));
        }

        return transformedVertices;
    }
    void setShader(Shader *shader)
    {
        this->shader = shader;
    }
    Shader *getShader()
    {
        return shader;
    }
    void setTexture(Texture* newTexture)
    {
        // delete texture;
        texture = newTexture;
    }
    Texture *getTexture()
    {
        return texture;
    }
    void setMesh(const UV_Mesh& newMesh) {
        if (isInitialized) {
            delete mesh;
            mesh = new UV_Mesh(newMesh);
            
            // Update vertex buffer
            if (vertexBuffer) {
                vertexBuffer->updateVertices(newMesh.vertices);
                vertexBuffer->updateIndices(newMesh.indices);
            } else {
                vertexBuffer = new UV_VertexBuffer(newMesh.vertices, newMesh.indices);
            }
        }
    }

    UV_Mesh *getMesh()
    {
        return mesh;
    }
    void setMode(MeshRendererMode mode)
    {
        this->mode = mode;
    }
    MeshRendererMode getMode()
    {
        return mode;
    }
    void setTransform(Transform *transform)
    {
        this->transform = transform;
    }
    Transform *getTransform()
    {
        return transform;
    }

private:
    Transform *transform;
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;

    bool isInitialized = false;
    MeshRendererMode mode;
    UV_Mesh *mesh;    // Changed to pointer
    Texture *texture; // Changed to pointer
    UV_VertexBuffer* vertexBuffer;
    Shader *shader;
};

#endif // MESH_RENDERER_H