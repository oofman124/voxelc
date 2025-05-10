#ifndef UV_VERTEX_BUFFER_H
#define UV_VERTEX_BUFFER_H

#include <glad/glad.h>
#include <vector>
#include "vertex.h"

class UV_VertexBuffer
{
public:
    UV_VertexBuffer(const std::vector<UV_Vertex>& vertices, const std::vector<unsigned int>& indices)
    {
        // Generate and bind VAO
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        // Generate and bind VBO
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(UV_Vertex), vertices.data(), GL_STATIC_DRAW);

        // Generate and bind EBO
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(UV_Vertex), (void*)0);
        glEnableVertexAttribArray(0);

        // Texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(UV_Vertex), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Unbind VAO
        glBindVertexArray(0);
    }

    ~UV_VertexBuffer()
    {
        // Clean up
        glBindVertexArray(0);
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
    bool isActive() const
    {
        GLint currentVAO;
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentVAO);
        return currentVAO == VAO;
    }

    void bind() const
    {
        glBindVertexArray(VAO);
    }

    void unbind() const
    {
        if (isActive())
            glBindVertexArray(0);
    }

    void updateVertices(const std::vector<UV_Vertex>& vertices)
    {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(UV_Vertex), vertices.data());
    }

    void updateIndices(const std::vector<unsigned int>& indices)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(unsigned int), indices.data());
    }

private:
    unsigned int VAO, VBO, EBO;
};

#endif // UV_VERTEX_BUFFER_H
