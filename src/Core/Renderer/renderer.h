#ifndef RENDERER_H
#define RENDERER_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <tuple>
#include "../Rendering/texture.h"
#include "../transform.h"
#include "../Rendering/vertexBuffer.h"
#include "../Rendering/texture.h"

class Renderer : public std::enable_shared_from_this<Renderer>
{
public:
    Renderer();
    ~Renderer();

    void initialize();
    GLFWwindow *getWindow() { return window; }
    void setInputMode(int mode, int value);
    void enableCapability(int capability);
    void disableCapability(int capability);
    void setBlendFunc(int sfactor, int dfactor);
    void setAutomaticViewport(bool enable);
    bool getAutomaticViewport() const { return automaticViewport; }
    void setViewport(int x, int y, int width, int height);
    void setShader(const std::string &shaderName);
    void setShader(std::shared_ptr<Shader> shader) { curShader = shader; }
    void setScrSize(unsigned int width, unsigned int height) { SCR_WIDTH = width; SCR_HEIGHT = height; }
    std::tuple<unsigned int, unsigned int> getScrSize() const { return std::make_tuple(SCR_WIDTH, SCR_HEIGHT); }
    void beginFrame(glm::mat4 viewMatrix);
    void renderMesh(std::shared_ptr<UV_VertexBuffer> buffer, std::shared_ptr<Texture> texture, std::shared_ptr<Transform> transform);
    void endFrame();
    void cleanup();

private:
    // Move constructor to private section and make it inline
    struct RenderBatch
    {
        std::weak_ptr<UV_VertexBuffer> buffer;
        std::weak_ptr<Texture> texture;
        std::weak_ptr<Transform> transform;
    };

    std::vector<RenderBatch> batches;

    GLFWwindow *window;
    std::shared_ptr<Shader> curShader;

    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;

    unsigned int SCR_WIDTH = 800;
    unsigned int SCR_HEIGHT = 600;

    bool isInitialized = false;
    bool isFrameStarted = false;
    bool automaticViewport = true;
};

#endif // RENDERER_H