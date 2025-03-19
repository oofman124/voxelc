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

enum MatrixType
{
    MODEL,
    VIEW,
    PROJECTION
};

class UV_MeshRenderer
{
public:
    UV_MeshRenderer(const UV_Mesh& mesh, Shader* shader, const Texture& texture)
        : mesh(mesh), shader(shader), texture(texture), vertexBuffer(mesh.vertices, mesh.indices)
    {
        if (mesh.vertices.empty() || mesh.indices.empty())
        {
            if (THROW_MESH_ERR)
                throw std::runtime_error("Mesh must have vertices and indices");
            return;
        }

        // Initialize matrices
        modelMatrix = glm::mat4(1.0f);
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
        shader->setMat4("model", modelMatrix);
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
            modelMatrix = matrix;
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
            matrix = modelMatrix;
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
    glm::mat4 modelMatrix;
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;

    UV_Mesh mesh;
    Texture texture;
    UV_VertexBuffer vertexBuffer;
    Shader* shader;
};

#endif // MESH_RENDERER_H