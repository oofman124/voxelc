#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <thread>
// #include "imgui.h"
// #include "imgui_impl_glfw.h"
// #include "imgui_impl_opengl3.h"

// #define STB_IMAGE_IMPLEMENTATION
// #include <stb_image.h>

#include "Core/Rendering/meshRenderer.h"
#include "Core/Rendering/mesh.h"
#include "Core/Rendering/shader.h"
#include "Core/transform.h"
#include "Core/instance.h"
#include "camera.h"
#include "World/world.h"
#include "Core/Math/frustrum.h"
#include "Core/assets.h"
#include "World/chunk.h"
#include "Core/UI/uiRenderer.h"
#include "Core/UI/uiRect.h"

static AssetManager &assetMgr = AssetManager::getInstance();
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const unsigned int RENDER_DISTANCE = 20;
// UI Renderer
std::shared_ptr<UIRenderer> uiRenderer = nullptr;

// timing
float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f;
// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
Frustum viewFrustum;

bool mouseLocked = true;

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

// Initialize GLFW and GLAD, then create window
GLFWwindow *initGLFW()
{
    if (!glfwInit())
    {
        std::cerr << "GLFW initialization failed!" << std::endl;
        exit(-1);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef _DEBUG
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    #endif

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "voxelc", NULL, NULL);
    if (!window)
    {
        std::cerr << "GLFW window creation failed!" << std::endl;
        glfwTerminate();
        exit(-1);
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    /* Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // IF using Docking Branch

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true); // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();
    */

    return window;
}

int main()
{
    // Initialize GLFW
    GLFWwindow *window = initGLFW();

    // Initialize GLAD to load OpenGL functions
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD!" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Now that GLAD is initialized, we can setup debug output
    #ifdef _DEBUG
        if (glfwExtensionSupported("GL_ARB_debug_output")) {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(debugMessage, nullptr);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        }
    #endif

    // Set up OpenGL options
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /* Create shader program
    Shader shader("resources/shaders/vertex_texture.glsl", "resources/shaders/fragment_texture.glsl");
    UV_Mesh mesh = Meshes::cube;
    UV_Mesh blockMesh = Meshes::block;

    Texture texture0("resources/textures/container.jpg", GL_TEXTURE0);
    Texture texture1("resources/textures/block_sample.png", GL_TEXTURE1); // This was a test for fallback

    Transform transform0;
    Transform transform1;
    transform1.setScale(glm::vec3(0.5f, 1.0f, 0.5f));
    transform1.setPosition(glm::vec3(1.0f, 1.0f, 0.0f));

    UV_MeshRenderer meshRenderer0(mesh, &shader, texture0, &transform0);
    UV_MeshRenderer meshRenderer1(blockMesh, &shader, texture1, &transform1);
    */
    assetMgr.initializeDefaultAssets();

    // Initialize UI Renderer
    uiRenderer = std::make_shared<UIRenderer>(SCR_WIDTH, SCR_HEIGHT);

    std::shared_ptr<Texture> texture0 = assetMgr.getTexture("grass");
    std::shared_ptr<Texture> texture1 = assetMgr.addTexture("block", "resources/textures/block_sample.png");
    // Shader shader("resources/shaders/vertex_texture.glsl", "resources/shaders/fragment_texture.glsl");
    // std::shared_ptr<Shader> shader = assetMgr.getShader("default");
    // World world;
    // world.generateTerrain(3, 3);
    // auto root = world.getRoot();

    // Create a colored rectangle
    // When creating colorRect
    auto colorRect = std::make_unique<UIRect>();
    colorRect->setPosition({10.0f, (float)SCR_HEIGHT - 60.0f}); // Adjust Y to start from top
    colorRect->setSize({100.0f, 50.0f});
    colorRect->setColor({1.0f, 0.0f, 0.0f, 1.0f}); // Red with full alpha
    // colorRect->setTexture(texture0);

    // Limit to 60 FPS
    const double frameTime = 1.0 / 60.0;
    const double stepTime = 1.0 / 5.0;
    double lastFrameTime = glfwGetTime();
    double lastStepTime = glfwGetTime();
    bool firstFrame = false;

    // Instance
    HierarchyMap<string, string> hierarchyMap;

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
            // hierarchyMap = root->exportToMap();
        }
        lastFrameTime = currentTime;

        // input
        // -----
        processInput(window);

        /* Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        */

        /*
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Quit", "Alt+Q"))
                {
                    glfwSetWindowShouldClose(window, true);
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View"))
            {
                if (ImGui::MenuItem("Explorer", "Alt+E"))
                {
                    explorerActive = !explorerActive;
                }
                ImGui::EndMenu();
            }
            if (mouseLocked)
            {
                if (ImGui::MenuItem("Toggle Mouse Lock (Alt+M)", "Alt+M", false))
                {
                    mouseLocked = !mouseLocked;
                    glfwSetInputMode(window, GLFW_CURSOR, mouseLocked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
                }
            }

            ImGui::EndMainMenuBar();
        }

        // Replace the existing Explorer window code with:
        if (explorerActive)
        {
            ImGui::Begin("Explorer", &explorerActive, ImGuiWindowFlags_MenuBar);

            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("Options"))
                {
                    if (ImGui::MenuItem("Refresh"))
                    {
                        hierarchyMap = root->exportToMap();
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }

            // Left panel: Tree view
            ImGui::BeginChild("Tree", ImVec2(ImGui::GetWindowWidth() * 0.5f, 0), true);
            hierarchyMap.renderImGuiTree("World");
            ImGui::EndChild();

            ImGui::SameLine();

            // Right panel: Properties
            ImGui::BeginChild("Properties", ImVec2(0, 0), true);
            if (auto selected = hierarchyMap.getSelected())
            {
                ImGui::Text("Properties for: %s", selected->data.c_str());
                ImGui::Separator();

                // Add property editing here
                // Example:
                if (ImGui::CollapsingHeader("Transform"))
                {
                    // Position, rotation, scale editors would go here
                }
            }
            else
            {
                ImGui::Text("No object selected");
            }
            ImGui::EndChild();

            ImGui::End();
        }
        */

        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        glm::mat4 projView = projection * view;
        // viewFrustum.update(projView);

        /* Find any descendants of the root instance that is a PVInstance and render them
        for (const auto &child : root->GetChildren())
        {
            auto chunk = std::dynamic_pointer_cast<Chunk>(child);
            if (chunk)
            {
                glm::vec3 position = chunk->getPosition();
                // float squaredDistance = glm::dot(position - camera.Position, position - camera.Position);
                float distanceMag = glm::length(position - camera.Position);
                if (distanceMag <= RENDER_DISTANCE squaredDistance < RENDER_DISTANCE * RENDER_DISTANCE)
                {
                    for (const auto &blk : chunk->GetChildren())
                    {
                        auto pvInstance = std::dynamic_pointer_cast<PVInstance>(blk);
                        if (pvInstance)
                        {
                            auto transform = pvInstance->transform.lock();
                            auto meshRenderer = pvInstance->meshRenderer.lock();
                            if (meshRenderer)
                            {
                                if (meshRenderer->getMode() == MESH_RENDERER_MODE_DEFAULT)
                                {
                                    meshRenderer->setMatrix(VIEW, view);
                                    meshRenderer->setMatrix(PROJECTION, projection);
                                }
                                meshRenderer->render();
                            }
                        }
                    }
                }
            }
            */

        /*
        auto pvInstance = std::dynamic_pointer_cast<PVInstance>(child);

        if (pvInstance)
        {
            auto transform = pvInstance->transform.lock();
            if (!transform)
                continue;

            /* disabled for now
            // Check if object is in view frustum
            glm::vec3 position = transform->getPosition();
            float radius = glm::length(transform->getScale()) * 2.0f; // Approximate bounding sphere

            if (!viewFrustum.isInFrustum(position, radius))
            {
                continue; // Skip rendering if not in frustum
            }
            auto meshRenderer = pvInstance->meshRenderer.lock();
            if (meshRenderer)
            {
                if (meshRenderer->getMode() == MESH_RENDERER_MODE_DEFAULT)
                {
                    meshRenderer->setMatrix(VIEW, view);
                    meshRenderer->setMatrix(PROJECTION, projection);
                }
                else if (meshRenderer->getMode() == MESH_RENDERER_MODE_2D)
                {
                    glm::vec3 pos = transform->getPosition();
                    transform->setPosition(glm::vec3(pos.x, pos.y, 0.0f));
                    meshRenderer->setMatrix(VIEW, view_2d);
                    meshRenderer->setMatrix(PROJECTION, projection_2d);
                }
                meshRenderer->render();
            }


            // Render distance limit
            float squaredDistance = glm::dot(transform->getPosition() - camera.Position, transform->getPosition() - camera.Position);
            if (squaredDistance < RENDER_DISTANCE * RENDER_DISTANCE)
            {
                auto meshRenderer = pvInstance->meshRenderer.lock();
                if (meshRenderer)
                {
                    if (meshRenderer->getMode() == MESH_RENDERER_MODE_DEFAULT)
                    {
                        meshRenderer->setMatrix(VIEW, view);
                        meshRenderer->setMatrix(PROJECTION, projection);
                    }
                    else if (meshRenderer->getMode() == MESH_RENDERER_MODE_2D)
                    {
                        glm::vec3 pos = transform->getPosition();
                        transform->setPosition(glm::vec3(pos.x, pos.y, 0.0f));
                        meshRenderer->setMatrix(VIEW, view_2d);
                        meshRenderer->setMatrix(PROJECTION, projection_2d);
                    }
                    meshRenderer->render();
                }
            }
        }
     */
        //}
        glDisable(GL_DEPTH_TEST);
        uiRenderer->begin();
        uiRenderer->submit(colorRect.get());
        uiRenderer->flush();
        glEnable(GL_DEPTH_TEST);
        // ImGui::Render();
        // ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
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
        // ImGui_ImplOpenGL3_Shutdown();
        // ImGui_ImplGlfw_Shutdown();
        // ImGui::DestroyContext();
        // root.reset(); // This will also delete all children instances and their resources
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        mouseLocked = !mouseLocked;
    glfwSetInputMode(window, GLFW_CURSOR, mouseLocked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    if (uiRenderer)
        uiRenderer->SetProjection(width, height);
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