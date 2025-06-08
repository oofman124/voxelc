#ifndef Vertex2D_BUFFER_H
#define Vertex2D_BUFFER_H

#include <glad/glad.h>
#include <vector>
#include <tuple>
#include <memory>
#include <iostream>
#include "vertex2D.h"

class VertexBuffer2D: public std::enable_shared_from_this<VertexBuffer2D>
{
public:
    VertexBuffer2D(const std::vector<Vertex2D>& vertices, const std::vector<unsigned int>& indices)
    : vertices(vertices), indices(indices)
    {
        // Generate and bind VAO
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        // Generate and bind VBO
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex2D), nullptr, GL_DYNAMIC_DRAW); // Will resize as needed

        // Generate and bind EBO
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), nullptr, GL_DYNAMIC_DRAW);

        // Vertex attribs: pos (2), uv (2), color (4)
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*)(sizeof(glm::vec2)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*)(2 * sizeof(glm::vec2)));
        glEnableVertexAttribArray(2);

        // Unbind VAO
        glBindVertexArray(0);
    }
    bool isValid() const {
        GLint maxVAO = 0;
        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVAO);
        
        if (VAO == 0) {
            std::cout << "Invalid VAO: " << VAO << std::endl;
            return false;
        }
        if (VBO == 0) {
            std::cout << "Invalid VBO: " << VBO << std::endl;
            return false;
        }
        if (EBO == 0) {
            std::cout << "Invalid EBO: " << EBO << std::endl;
            return false;
        }
        if (vertices.empty()) {
            std::cout << "Empty vertices array" << std::endl;
            return false;
        }
        if (indices.empty()) {
            std::cout << "Empty indices array" << std::endl;
            return false;
        }
        return true;
    }

    ~VertexBuffer2D()
    {
        // Clean up
        glBindVertexArray(0);
        //std::cout << "Deleting buffers: " << VAO << ", " << VBO << ", " << EBO << std::endl;
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

    void updateVertices(const std::vector<Vertex2D>& vertices)
    {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex2D), vertices.data());
    }

    void updateIndices(const std::vector<unsigned int>& indices)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(unsigned int), indices.data());
    }

    std::tuple<unsigned int, unsigned int, unsigned int> getBuffers() const
    {
        return std::make_tuple(VAO, VBO, EBO);
    }

    const std::vector<unsigned int>& getIndices() const
    {
        return indices;
    }
    unsigned int getIndexCount() const
    {
        return static_cast<unsigned int>(indices.size());
    }

private:
    unsigned int VAO, VBO, EBO;
    const std::vector<Vertex2D>& vertices;
    const std::vector<unsigned int>& indices;
};

#endif // Vertex2D_BUFFER_H
