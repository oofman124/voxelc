#ifndef RENDERER2D_H
#define RENDERER2D_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include "../Rendering/shader.h"
#include "../Rendering/texture.h"
#include "../Rendering/vertexBuffer2D.h"
#include "../Util/vertex.h"

struct Quad2D {
    glm::vec2 position;
    glm::vec2 size;
    glm::vec4 color;
    std::shared_ptr<Texture> texture;
};


class Renderer2D: public std::enable_shared_from_this<Renderer2D>
{
public:
    Renderer2D();
    ~Renderer2D();

    void setShader(std::shared_ptr<Shader> shader);
    void setProjection(const glm::mat4& proj);

    void beginFrame();
    void endFrame();

    void drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
    void drawQuad(const glm::vec2& position, const glm::vec2& size, std::shared_ptr<Texture> texture, const glm::vec4& color = glm::vec4(1.0f));

private:
    void flush();

    std::vector<Quad2D> batch;
    std::shared_ptr<Shader> shader;
    GLuint VAO, VBO, EBO;
    std::shared_ptr<VertexBuffer2D> vertexBuffer;
    bool initialized = false;
    glm::mat4 projection = glm::mat4(1.0f);
};

#endif // RENDERER2D_H