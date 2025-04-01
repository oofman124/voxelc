#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// #define STB_IMAGE_IMPLEMENTATION
// #include <stb_image.h>

#include "Core/Rendering/meshRenderer.h"
#include "Core/Rendering/mesh.h"
#include "Core/Rendering/shader.h"
#include "core/transform.h"
#include "camera.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Cube vertices: position (x, y, z), texture coordinates (u, v)
float vertices[] = {
    // Front face
    -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,  // Bottom left  
     0.5f, -0.5f,  0.5f,   1.0f, 0.0f,  // Bottom right 
     0.5f,  0.5f,  0.5f,   1.0f, 1.0f,  // Top right    
    -0.5f,  0.5f,  0.5f,   0.0f, 1.0f,  // Top left     

    // Back face
    -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,  // Bottom left  
     0.5f, -0.5f, -0.5f,   1.0f, 0.0f,  // Bottom right 
     0.5f,  0.5f, -0.5f,   1.0f, 1.0f,  // Top right    
    -0.5f,  0.5f, -0.5f,   0.0f, 1.0f,  // Top left     

    // Left face
    -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,  // Bottom left
    -0.5f, -0.5f,  0.5f,   1.0f, 0.0f,  // Bottom right
    -0.5f,  0.5f,  0.5f,   1.0f, 1.0f,  // Top right
    -0.5f,  0.5f, -0.5f,   0.0f, 1.0f,  // Top left

    // Right face
     0.5f, -0.5f, -0.5f,   0.0f, 0.0f,  // Bottom left
     0.5f, -0.5f,  0.5f,   1.0f, 0.0f,  // Bottom right
     0.5f,  0.5f,  0.5f,   1.0f, 1.0f,  // Top right
     0.5f,  0.5f, -0.5f,   0.0f, 1.0f,  // Top left

     // Top face
     -0.5f,  0.5f, -0.5f,   0.0f, 0.0f,  // Bottom left
      0.5f,  0.5f, -0.5f,   1.0f, 0.0f,  // Bottom right
      0.5f,  0.5f,  0.5f,   1.0f, 1.0f,  // Top right
     -0.5f,  0.5f,  0.5f,   0.0f, 1.0f,  // Top left

     // Bottom face
     -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,  // Bottom left
      0.5f, -0.5f, -0.5f,   1.0f, 0.0f,  // Bottom right
      0.5f, -0.5f,  0.5f,   1.0f, 1.0f,  // Top right
     -0.5f, -0.5f,  0.5f,   0.0f, 1.0f   // Top left
};

// Indices for drawing the cube faces
unsigned int indices[] = {
    // Front
    0, 1, 2,   2, 3, 0,
    // Back
    4, 5, 6,   6, 7, 4,
    // Left
    8, 9, 10,  10, 11, 8,
    // Right
    12, 13, 14, 14, 15, 12,
    // Top
    16, 17, 18, 18, 19, 16,
    // Bottom
    20, 21, 22, 22, 23, 20
};

glm::vec3 cubePositions[] = {
    glm::vec3(0.0f,  0.0f,  0.0f),
    glm::vec3(2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3(2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f,  3.0f, -7.5f),
    glm::vec3(1.3f, -2.0f, -2.5f),
    glm::vec3(1.5f,  2.0f, -2.5f),
    glm::vec3(1.5f,  0.2f, -1.5f),
    glm::vec3(-1.3f,  1.0f, -1.5f)
};


// Initialize GLFW and GLAD, then create window
GLFWwindow* initGLFW() {
    if (!glfwInit()) {
        std::cerr << "GLFW initialization failed!" << std::endl;
        exit(-1);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Modern OpenGL Cube", NULL, NULL);
    if (!window) {
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
    return window;
}

int main() {
    // Initialize GLFW
    GLFWwindow* window = initGLFW();

    // Initialize GLAD to load OpenGL functions
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD!" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Set up OpenGL options
    glEnable(GL_DEPTH_TEST);

    // Create shader program
    Shader shader("resources/shaders/vertex_texture.glsl", "resources/shaders/fragment_texture.glsl");
	UV_Mesh mesh = Meshes::cube;
    UV_Mesh blockMesh = Meshes::block;

	Texture texture0("resources/textures/container.jpg", GL_TEXTURE0);
    Texture texture1("resources/textures/block_sample_jpeg.jpg", GL_TEXTURE1);

    Transform transform0;
    Transform transform1;
    transform1.setScale(glm::vec3(0.5f, 1.0f, 0.5f));
    transform1.setPosition(glm::vec3(1.0f, 1.0f, 0.0f));

    UV_MeshRenderer meshRenderer0(mesh, &shader, texture0, &transform0);
    UV_MeshRenderer meshRenderer1(blockMesh, &shader, texture1, &transform1);


    /*
    * // Generate VBO, VAO, and EBO for the cube
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    */

    /*
    // load and create a texture 
    // -------------------------
    unsigned int texture;
    // texture 1
    // ---------
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    unsigned char* data = stbi_load("resources/textures/container.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else
        std::cout << "Failed to load texture" << std::endl;
    stbi_image_free(data);

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
   // -------------------------------------------------------------------------------------------
    shader.use(); // don't forget to activate/use the shader before setting uniforms!
    // either set it manually like so:
    // glUniform1i(glGetUniformLocation(shader.ID, "texture0"), 0);
    // or set it via the texture class
    shader.setInt("texture0", 0);
    */
    
   


    // Main rendering loop
    while (!glfwWindowShouldClose(window)) {

        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;


        // input
        // -----
        processInput(window);

        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        float angle = glm::radians(50.0f) * deltaTime; // 50 degrees per second
        transform1.setRotation(transform1.getRotation() * glm::quat(glm::vec3(0.0f, angle, 0.0f)));

        /*
        glm::mat4 model0 = glm::mat4(1.0f);
        model0 = glm::translate(model0, cubePositions[0]);
        glm::mat4 model1 = glm::mat4(1.0f);
        model1 = glm::translate(model1, cubePositions[1]);
        */

        meshRenderer0.setMatrix(VIEW, view);
        meshRenderer1.setMatrix(VIEW, view);
		meshRenderer0.setMatrix(PROJECTION, projection);
        meshRenderer1.setMatrix(PROJECTION, projection);


		// meshRenderer0.setMatrix(MODEL, model0);
        // meshRenderer1.setMatrix(MODEL, model1);
		meshRenderer1.render();
        meshRenderer0.render();

        /*
         // activate shader
        shader.use();

        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        shader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("view", view);


            unsigned int modelLoc = glGetUniformLocation(shader.ID, "model");
            unsigned int viewLoc = glGetUniformLocation(shader.ID, "view");
            unsigned int projectionLoc = glGetUniformLocation(shader.ID, "projection");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
  

        // Draw the cube
        glBindVertexArray(VAO);
        // glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        for (unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            shader.setMat4("model", model);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }
        */
       

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
    //cleanup
    try
    {
        texture0.~Texture();
        texture1.~Texture();
		meshRenderer0.~UV_MeshRenderer();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
    }


    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
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
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
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
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}