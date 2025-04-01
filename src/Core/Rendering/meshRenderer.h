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

enum MatrixType
{
    MODEL,
    VIEW,
    PROJECTION
};

class UV_MeshRenderer
{
public:
    UV_MeshRenderer(const UV_Mesh& mesh, Shader* shader, const Texture& texture, Transform* transform)
        : mesh(mesh), shader(shader), texture(texture), vertexBuffer(mesh.vertices, mesh.indices), transform(transform)
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

        // Initialize matrices
        viewMatrix = glm::mat4(1.0f);
        projectionMatrix = glm::mat4(1.0f);
    }

    ~UV_MeshRenderer()
    {
        // Clean up
        vertexBuffer.~UV_VertexBuffer();
    }

    void render()
    {
        // Use the shader
        shader->use();

        // Set the shader uniforms
        shader->setMat4("model", transform->getMatrix());
        shader->setMat4("view", viewMatrix);
        shader->setMat4("projection", projectionMatrix);

        // Bind the texture
        texture.bindToShaderInt(*shader, "texture0");

        // Bind the vertex buffer and draw the mesh
        vertexBuffer.bind();
        glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
        vertexBuffer.unbind();
    }

    void setMatrix(MatrixType type, const glm::mat4& matrix)
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

    void getMatrix(MatrixType type, glm::mat4& matrix)
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

private:
    // glm::mat4 modelMatrix;
    Transform* transform;
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;

    UV_Mesh mesh;
    Texture texture;
    UV_VertexBuffer vertexBuffer;
    Shader* shader;
};

#endif // MESH_RENDERER_H