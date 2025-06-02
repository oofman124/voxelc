#include "renderer2D.h"
#include <glm/gtc/matrix_transform.hpp>
// , projection(glm::ortho(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f)
Renderer2D::Renderer2D() : VAO(0), VBO(0), EBO(0), initialized(false) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex2D), nullptr, GL_DYNAMIC_DRAW); // Will resize as needed

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), nullptr, GL_DYNAMIC_DRAW);

    // Vertex attribs: pos (2), uv (2), color (4)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*)(sizeof(glm::vec2)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*)(2 * sizeof(glm::vec2)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    initialized = true;
}

Renderer2D::~Renderer2D() {
    if (initialized) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
}

void Renderer2D::setProjection(const glm::mat4& proj) {
    projection = proj;
}

void Renderer2D::setShader(std::shared_ptr<Shader> shader) {
    this->shader = shader;
}

void Renderer2D::beginFrame() {
    batch.clear(); // Exception has occurred: W32/0xC0000005
                   //Unhandled exception thrown: read access violation.
                   //_Mylast was 0x10.
}

void Renderer2D::endFrame() {
    flush();
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cout << "OpenGL error: " << err << std::endl;
    }
}

void Renderer2D::drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color) {
    batch.push_back({ position, size, color, nullptr });
}

void Renderer2D::drawQuad(const glm::vec2& position, const glm::vec2& size, std::shared_ptr<Texture> texture, const glm::vec4& color) {
    batch.push_back({ position, size, color, texture });
}

void Renderer2D::flush() {
    if (!shader || batch.empty()) return;
    shader->use();
    shader->setMat4("projection", projection);
    std::cout << "Flushing " << batch.size() << " quads\n";

    std::vector<Vertex2D> vertices;
    std::vector<unsigned int> indices;
    vertices.reserve(batch.size() * 4);
    indices.reserve(batch.size() * 6);

    for (size_t i = 0; i < batch.size(); ++i) {
        const auto& quad = batch[i];
        glm::vec2 p = quad.position;
        glm::vec2 s = quad.size;
        glm::vec4 c = quad.color;

        // 4 corners (CCW): top-left, top-right, bottom-right, bottom-left
        vertices.push_back({ p,               {0.0f, 1.0f}, c });
        vertices.push_back({ p + glm::vec2(s.x, 0), {1.0f, 1.0f}, c });
        vertices.push_back({ p + s,           {1.0f, 0.0f}, c });
        vertices.push_back({ p + glm::vec2(0, s.y), {0.0f, 0.0f}, c });

        unsigned int base = static_cast<unsigned int>(i * 4);
        indices.push_back(base + 0);
        indices.push_back(base + 1);
        indices.push_back(base + 2);
        indices.push_back(base + 2);
        indices.push_back(base + 3);
        indices.push_back(base + 0);
    }

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex2D), vertices.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);

    size_t quadCount = batch.size();
    size_t indexOffset = 0;
    for (size_t i = 0; i < quadCount; ++i) {
        const auto& quad = batch[i];
        if (quad.texture) {
            quad.texture->bindToShaderInt(*shader, "texture0");
            shader->setInt("useTexture", 1);
        } else {
            shader->setInt("useTexture", 0);
        }
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(indexOffset * sizeof(unsigned int)));
        if (quad.texture) quad.texture->unbind();
        indexOffset += 6;
    }

    glBindVertexArray(0);
}