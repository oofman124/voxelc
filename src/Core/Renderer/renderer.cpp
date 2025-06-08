#include <stdexcept>
#include <glm/gtc/matrix_transform.hpp>
#include "renderer.h"
#include "../assets.h"


Renderer::Renderer() : 
    window(nullptr),
    curShader(nullptr),
    viewMatrix(1.0f),
    projectionMatrix(glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.1f, 1000.0f))
{
}
Renderer::~Renderer() {
    cleanup();
}


void framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    Renderer *renderer = static_cast<Renderer *>(glfwGetWindowUserPointer(window));
    if (renderer && renderer->getAutomaticViewport())
    {
        renderer->setScrSize(width, height);
        glViewport(0, 0, width, height);
    }
}

void APIENTRY debugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
    // Some debug messages are just annoying informational messages
    switch (id)
    {
    case 131185: // glBufferData
        return;
    }

    printf("Message: %s\n", message);
    printf("Source: ");

    switch (source)
    {
    case GL_DEBUG_SOURCE_API:
        printf("API");
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        printf("Window System");
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        printf("Shader Compiler");
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        printf("Third Party");
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        printf("Application");
        break;
    case GL_DEBUG_SOURCE_OTHER:
        printf("Other");
        break;
    }

    printf("\n");
    printf("Type: ");

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:
        printf("Error");
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        printf("Deprecated Behavior");
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        printf("Undefined Behavior");
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        printf("Portability");
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        printf("Performance");
        break;
    case GL_DEBUG_TYPE_MARKER:
        printf("Marker");
        break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        printf("Push Group");
        break;
    case GL_DEBUG_TYPE_POP_GROUP:
        printf("Pop Group");
        break;
    case GL_DEBUG_TYPE_OTHER:
        printf("Other");
        break;
    }

    printf("\n");
    printf("ID: %d\n", id);
    printf("Severity: ");

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:
        printf("High");
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        printf("Medium");
        break;
    case GL_DEBUG_SEVERITY_LOW:
        printf("Low");
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        printf("Notification");
        break;
    }

    printf("\n\n");
}



void Renderer::initialize()
{
    if (isInitialized)
    {
        return;
    }

    // Initialize GLFW
    if (!glfwInit())
    {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE); // GLFW_OPENGL_CORE_PROFILE
#ifdef _DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "voxelc", nullptr, nullptr);
    glfwSetWindowUserPointer(window, reinterpret_cast<void *>(this));
    if (!window)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        throw std::runtime_error("Failed to initialize GLAD");
    }

    // Now that GLAD is initialized, we can setup debug output
#ifdef _DEBUG
if (glfwExtensionSupported("GL_ARB_debug_output"))
{
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(debugMessage, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
}
#endif


    // Set default OpenGL state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    isInitialized = true;
}

void Renderer::setInputMode(int mode, int value)
{
    if (!window)
        return;
    glfwSetInputMode(window, mode, value);
}

void Renderer::enableCapability(int capability)
{
    glEnable(capability);
}

void Renderer::disableCapability(int capability)
{
    glDisable(capability);
}

void Renderer::setBlendFunc(int sfactor, int dfactor)
{
    glBlendFunc(sfactor, dfactor);
}

void Renderer::setAutomaticViewport(bool enable)
{
    automaticViewport = enable;
}

void Renderer::setViewport(int x, int y, int width, int height)
{
    glViewport(x, y, width, height);
}

void Renderer::setShader(const std::string &shaderName)
{
    auto &assetMgr = AssetManager::getInstance();
    curShader = assetMgr.getShader(shaderName);
    if (!curShader)
    {
        throw std::runtime_error("Shader not found: " + shaderName);
    }
}

void Renderer::beginFrame(glm::mat4 viewMatrix)
{
    if (isFrameStarted)
    {
        throw std::runtime_error("Frame already in progress");
    }

    this->viewMatrix = viewMatrix;
    batches.clear();
    isFrameStarted = true;

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::renderMesh(std::shared_ptr<UV_VertexBuffer> buffer,
                          std::shared_ptr<Texture> texture,
                          std::shared_ptr<Transform> transform)
{
    if (!isFrameStarted)
    {
        throw std::runtime_error("Frame not started");
    }
    // Validate resources before queueing
    if (!buffer || !buffer->isValid() || !texture || !transform) {
        return;
    }


    // Add to render batch
    RenderBatch batch{
        buffer,
        texture,
        transform};
    batches.push_back(batch);
}

void Renderer::endFrame()
{
    if (!isFrameStarted)
    {
        throw std::runtime_error("No frame in progress");
    }

    if (!curShader)
    {
        throw std::runtime_error("No shader set");
    }

    // Render all batches
    curShader->use();
    curShader->setMat4("view", viewMatrix);
    curShader->setMat4("projection", projectionMatrix);

    for (const auto &batch : batches)
    {
        auto buffer = batch.buffer.lock();
        auto texture = batch.texture.lock();
        auto transform = batch.transform.lock();
        // Skip invalid batches
        if (!buffer || !buffer->isValid() || !texture || !transform) {
            continue;
        }
        if (texture)
        {
            texture->bind();
            texture->bindToShaderInt(*curShader, "texture0");
        }

        curShader->setMat4("model", transform->getMatrix());

        buffer->bind();
        //std::cout << "Drawing " << buffer->getIndexCount() << " indices" << std::endl;
        //auto buffers = buffer->getBuffers();
        /*
        std::cout << "Buffer IDs: (" 
                  << std::get<0>(buffers) << ", " 
                  << std::get<1>(buffers) << ", " 
                  << std::get<2>(buffers) << ")" << std::endl;
        */
        glDrawElements(GL_TRIANGLES, buffer->getIndexCount(), GL_UNSIGNED_INT, 0);
        buffer->unbind();

        if (texture)
        {
            texture->unbind();
        }
    }

    //glfwSwapBuffers(window);
    isFrameStarted = false;
}

void Renderer::cleanup()
{
    if (window)
    {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();
    isInitialized = false;
}