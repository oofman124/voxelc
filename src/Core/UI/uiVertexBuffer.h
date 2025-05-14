#ifndef UI_VERTEX_BUFFER_H
#define UI_VERTEX_BUFFER_H

#include <glad/glad.h>
#include <vector>
#include "uiVertex.h"

class UIVertexBuffer
{
public:
    // Constructor with initial data
    UIVertexBuffer(const std::vector<UIVertex> &vertices, const std::vector<unsigned int> &indices)
    {
        initializeBuffers();
        updateData(vertices, indices);
    }

    // Default constructor
    UIVertexBuffer()
    {
        initializeBuffers();
    }

    ~UIVertexBuffer()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    void bind() const
    {
        glBindVertexArray(VAO);
    }

    void unbind() const
    {
        glBindVertexArray(0);
    }

    void updateData(const std::vector<UIVertex> &vertices, const std::vector<uint32_t> &indices)
    {
        if (vertices.empty() || indices.empty())
            return;

        bind();

        // Update vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // First allocation with glBufferData if buffer size changes
        GLint currentSize;
        glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &currentSize);
        if (currentSize < static_cast<GLint>(vertices.size() * sizeof(UIVertex)))
        {
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(UIVertex),
                         nullptr, GL_DYNAMIC_DRAW);
        }
        // Then update with glBufferSubData
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(UIVertex),
                        vertices.data());

        // Update index buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &currentSize);
        if (currentSize < static_cast<GLint>(indices.size() * sizeof(uint32_t)))
        {
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t),
                         nullptr, GL_DYNAMIC_DRAW);
        }
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(uint32_t),
                        indices.data());

        unbind();
    }

private:
    unsigned int VAO, VBO, EBO;

    void initializeBuffers()
    {
        // Generate and bind VAO
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        // Generate buffers
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        // Bind buffers
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

        // Set up vertex attributes
        // Position (x, y)
         glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(UIVertex), (void*)0); // offset for x,y
        glEnableVertexAttribArray(0);

        // Texture coordinates (u, v)
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(UIVertex), (void*)(2 * sizeof(float))); // offset after x,y
        glEnableVertexAttribArray(1);

        // Color (r, g, b, a)
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(UIVertex), (void*)(4 * sizeof(float))); // offset after x,y,u,v

        glEnableVertexAttribArray(2);

        // Unbind
        glBindVertexArray(0);
    }
};

#endif // UI_VERTEX_BUFFER_H