#ifndef UV_VERTEX_BUFFER_H
#define UV_VERTEX_BUFFER_H

#include <glad/glad.h>
#include <vector>
#include "../Util/vertex.h"
#include <tuple>

class UV_VertexBuffer: public std::enable_shared_from_this<UV_VertexBuffer>
{
public:
    UV_VertexBuffer(const std::vector<Vertex> vertices, const std::vector<unsigned int> indices)
    : vertices(vertices), indices(indices)
    {
        // Generate and bind VAO
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        // Generate and bind VBO
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        // Generate and bind EBO
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);

        // Texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

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

    ~UV_VertexBuffer()
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

    void updateVertices(const std::vector<Vertex> &newVertices)
    {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        if (newVertices.size() * sizeof(Vertex) != vertices.size() * sizeof(Vertex)) {
            // Reallocate buffer if size changed
            glBufferData(GL_ARRAY_BUFFER, newVertices.size() * sizeof(Vertex), newVertices.data(), GL_STATIC_DRAW);
        }
        else {
            glBufferSubData(GL_ARRAY_BUFFER, 0, newVertices.size() * sizeof(Vertex), newVertices.data());
        }
        vertices = newVertices;
    }

    void updateIndices(const std::vector<unsigned int> &newIndices)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        if (newIndices.size() * sizeof(unsigned int) != indices.size() * sizeof(unsigned int)) {
            // Reallocate buffer if size changed
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, newIndices.size() * sizeof(unsigned int), newIndices.data(), GL_STATIC_DRAW);
        }
        else {
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, newIndices.size() * sizeof(unsigned int), newIndices.data());
        }
        indices = newIndices;
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
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
};

#endif // UV_VERTEX_BUFFER_H
