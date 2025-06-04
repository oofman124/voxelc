#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <thread>

#include "Core/Rendering/meshRenderer.h"
#include "Core/Rendering/mesh.h"
#include "Core/Rendering/shader.h"
#include "Core/transform.h"
#include "Core/object.h"
#include "Core/camera.h"
#include "Core/World/world.h"
#include "Core/Math/frustrum.h"
#include "Core/assets.h"
#include "Core/World/chunk.h"
#include "Core/Renderer/renderer.h"
#include "Core/Renderer/renderer2D.h"
#include "Core/Block/blockDatabase.h"
#include "Core/Input/InputManager.h"

static AssetManager &assetMgr = AssetManager::getInstance();
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const unsigned int RENDER_DISTANCE = 16 * 3;
// UI Renderer
// std::shared_ptr<UIRenderer> uiRenderer = nullptr;
std::shared_ptr<Renderer> renderer = nullptr;
std::shared_ptr<Renderer2D> uiRenderer = nullptr;

// timing
float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f;
// camera
Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
Frustum viewFrustum;

bool mouseLocked = true;

int main()
{

    renderer = std::make_shared<Renderer>();
    renderer->initialize();

    GLFWwindow *window = renderer->getWindow();
    // Set up OpenGL callbacks related to input
    // glfwSetCursorPosCallback(window, mouse_callback);
    // glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    InputManager::initialize(window);

    // Set up OpenGL options
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    assetMgr.initializeDefaultAssets();
    if (assetMgr.getTextureAtlas("terrain_atlas"))
        BlockDatabase::initialize(assetMgr.getTextureAtlas("terrain_atlas"));

    // Initialize UI Renderer
    uiRenderer = std::make_shared<Renderer2D>();
    uiRenderer->setShader(assetMgr.getShader("ui"));
    uiRenderer->setProjection(glm::ortho(0.0f, (float)SCR_WIDTH, (float)SCR_HEIGHT, 0.0f, -1.0f, 1.0f));

    // std::shared_ptr<Texture> texture0 = assetMgr.getTexture("grass");
    // std::shared_ptr<Texture> texture1 = assetMgr.addTexture("block", "resources/textures/block_sample.png");
    // Shader shader("resources/shaders/vertex_texture.glsl", "resources/shaders/fragment_texture.glsl");
    std::shared_ptr<Shader> shader = assetMgr.getShader("default");
    renderer->setShader(shader);
    std::shared_ptr<World> world = std::make_shared<World>();
    auto root = world->getRoot();

    std::atomic<bool> shouldStop{false};

    // Generate terrain on main thread first
    world->generateTerrain(12, 12);


    InputManager::onKeyPressed([window](int key) {
        if (key == GLFW_KEY_ESCAPE) {
        InputManager::setMouseLocked(!InputManager::isMouseLocked());
        if (InputManager::isMouseLocked()) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            lastX = static_cast<float>(xpos);
            lastY = static_cast<float>(ypos);
            firstMouse = false;
        }
    }});
    InputManager::onScroll([](double xoffset, double yoffset) {
        if (InputManager::isMouseLocked() || InputManager::isMouseButtonDown(GLFW_MOUSE_BUTTON_RIGHT)) {
            camera.ProcessMouseScroll(yoffset);
        }
    });
    InputManager::onMouseButtonPressed([window](int button) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        lastX = static_cast<float>(xpos);
        lastY = static_cast<float>(ypos);
        firstMouse = false;
    }});
    InputManager::onCursorPos([](double xpos, double ypos) {
        if (InputManager::isMouseLocked() || InputManager::isMouseButtonDown(GLFW_MOUSE_BUTTON_RIGHT)) {
            if (firstMouse)
            {
                lastX = static_cast<float>(xpos);
                lastY = static_cast<float>(ypos);
                firstMouse = false;
            }
            float xoffset = static_cast<float>(xpos - lastX);
            float yoffset = static_cast<float>(lastY - ypos); // reversed since y-coordinates go from bottom to top
            lastX = static_cast<float>(xpos);
            lastY = static_cast<float>(ypos);
            camera.ProcessMouseMovement(xoffset, yoffset);
        }
    });


    // Start update thread
    /*
    std::thread updateThread([&world, &shouldStop]() {
        const double updateInterval = 1.0 / 30.0; // 30 updates per second
        world->generateTerrain(2,2);
         world->update();
        while (!shouldStop.load()) {
            auto start = std::chrono::steady_clock::now();

            //world->update();

            auto end = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration<double>(end - start);
            auto sleepTime = updateInterval - elapsed.count();

            if (sleepTime > 0) {
                std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime));
            }
        }
    });
    updateThread.detach();
    */

    // Limit to 60 FPS
    const double frameTime = 1.0 / 60.0;
    const double stepTime = 1.0 / 5.0;
    double lastFrameTime = glfwGetTime();
    double lastStepTime = glfwGetTime();
    bool firstFrame = false;

    // ImGui globals
    bool explorerActive = false;

    // Main rendering loop
    while (!glfwWindowShouldClose(window))
    {

        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        double currentTime = glfwGetTime();
        double elapsedTime = currentTime - lastFrameTime;

        if (deltaTime < frameTime)
        {
            double sleepTime = frameTime - deltaTime;
            std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime));
            currentTime = glfwGetTime();
            deltaTime = currentTime - lastFrameTime;
        }
        if ((lastStepTime - currentTime) < stepTime)
        {
            lastStepTime = glfwGetTime();
        }
        lastFrameTime = currentTime;


        // render
        // glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        world->tickUpdate();

        glm::mat4 view = camera.GetViewMatrix();

        auto chunks = world->getChunksInRange(camera.Position, RENDER_DISTANCE);

        renderer->beginFrame(view);
        // Find any descendants of the root object that is a PVObject and render them
        for (const auto &chunk : chunks)
        {
            if (!chunk->isReady())
                continue;
            chunk->queueToRenderer(renderer);
        }
        renderer->endFrame();

        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDisable(GL_CULL_FACE);
        uiRenderer->beginFrame();
        // Render UI elements here
        uiRenderer->drawQuad(glm::vec2(1, 1), glm::vec2(200, 200), assetMgr.getTexture("notch"), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        uiRenderer->endFrame();
        glEnable(GL_DEPTH_TEST);

        glfwSwapBuffers(window);
        glfwPollEvents();

        // input
        // -----
        InputManager::pollEvents();
        processInput(window);
    }
    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    /*
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    */
    // cleanup
    try
    {
        shouldStop.store(true);
        world.reset();
        renderer->cleanup();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}

static bool escPressedLastFrame = false;

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (InputManager::isInitialized())
    {

        if (InputManager::isKeyDown(GLFW_KEY_W))
            camera.ProcessKeyboard(FORWARD, deltaTime);
        if (InputManager::isKeyDown(GLFW_KEY_S))
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (InputManager::isKeyDown(GLFW_KEY_A))
            camera.ProcessKeyboard(LEFT, deltaTime);
        if (InputManager::isKeyDown(GLFW_KEY_D))
            camera.ProcessKeyboard(RIGHT, deltaTime);
    }
    /*
    bool escPressedThisFrame = glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS;

    if (escPressedThisFrame && !escPressedLastFrame)
    {
        mouseLocked = !mouseLocked;
        glfwSetInputMode(window, GLFW_CURSOR, mouseLocked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    }

    escPressedLastFrame = escPressedThisFrame;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    */
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    // if (uiRenderer)
    //     uiRenderer->SetProjection(width, height);
}
// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}