// main.cpp
// Created 3/5/26

// -------------------------------------------------------------------------------
// LIBRARIES

// External
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// #include <GL/gl.h>
# include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Internal
#include "shaderClass.h"
#include "vbo.h"
#include "vao.h"
#include "ebo.h"
#include "texture.h"
#include "camera.h"
#include "Mesh.h"
#include "object.h"

// std
#include <iostream>
#include <string>

// ------------------------------------------------------------------------------------------------------
// GLOBAL CONFIGURATION

constexpr int  WIDTH  = 800;
constexpr int  HEIGHT = 600;
constexpr char WINDOW_NAME[] = "Window";

//glm::vec4  bgColor     = glm::vec4(0.07f, 0.13f, 0.17f, 1.0f);
glm::vec4   bgColor     = glm::vec4(0.02f, 0.01f, 0.04f, 1.0f);
std::string objectFile  = "ISD.obj";
std::string textureFile = "ISD_hull_color_baked.png";

// ----------------------------------------------------
// PROCESS INPUT (Exit on Esc)
void processInput (GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)  {
        std::cout << "User pressed ESC\n";
        glfwSetWindowShouldClose(window, true);
    }
}

// =======================================================================================================
int main() {

    // UNIFORM NAMES
    const char* texUniform = "tex0";
    glm::mat4 modelMatrix = glm::mat4(1.0f);


    //------------------------------------------------------------------------------------------------------
    // APPLICATION SETUP

    std::cout << "\nHello OpenGL!\n";

    // Initialize GLFW
    if (!glfwInit()) return -1;

    // Specify OpenGL version/profile for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Core: lacks deprecated functions

    // Create a GLFW window object
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, WINDOW_NAME, nullptr, nullptr);
    if (!window) { glfwTerminate(); return -1; }

    // Introduce window to the current context
    glfwMakeContextCurrent(window);

    // Load GLAD -> configures OpenGl to be driver agnostic
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD\n"; return -1; }

    // Define viewport size
    glViewport(0, 0, WIDTH, HEIGHT);

    //-------------------------------------------------------------------------------------
    // LOAD SHADER
    Shader shaderProgram("default.vert", "default.frag");
    shaderProgram.Activate();

    //----------------------------------------------------------------------------------------------------
    // LOAD MODEL
    std::string objFilePath = "resources/models/" + objectFile;
    Mesh mesh(objFilePath);

    //-------------------------------------------------------------------------------------
    // LOAD TEXTURE

    int numColorCh;
    std::string filePath = "resources/textures/" + textureFile;
    GLenum texType       = GL_TEXTURE_2D;
    GLenum texSlot       = GL_TEXTURE0;
    GLenum pixelType     = GL_UNSIGNED_BYTE;
    Tex texture = Tex(filePath.c_str(), texType, texSlot, pixelType);
    texture.setUniform(shaderProgram, texUniform, 0);

    //-----------------------------------------------------------------------------
    // DEPTH
    glEnable(GL_DEPTH_TEST);
    Camera camera(WIDTH, HEIGHT, glm::vec3(0.0f, 0.0f, 2.f));

    //-----------------------------------------------------------------------------
    // WIREFRAME RENDERING
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    float lastTime = glfwGetTime();

    //------------------------------------------------------------------------
    // OBJECT LIST
    std::vector<Object> objects;

    // OBJECT 1 (ISD)
    Object object1(mesh, texture);
    objects.push_back(object1);

    // OBJECT 2 (CUBE)
    Mesh cube_mesh("resources/models/cube.obj");
    Tex  cube_texture("resources/textures/osaka.png", texType, texSlot, pixelType);
    texture.setUniform(shaderProgram, texUniform, 0);
    Object buffer2(cube_mesh, cube_texture);
    buffer2.position.x = 1.0f;
    buffer2.scale = glm::vec3(100.0f, 100.0f, 100.0f);
    objects.push_back(buffer2);

    //===================================================================================================
    // Main Render Loop
    // --------------------------------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        // Background color
        glClearColor(bgColor.r, bgColor.g, bgColor.b, bgColor.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Activate Shader
        shaderProgram.Activate();

        // Camera Timing
        float currentTime = glfwGetTime();
        float deltaTime   = currentTime - lastTime;
        lastTime          = currentTime;
        camera.Inputs(window, deltaTime);

        // Camera Updating
        camera.UpdateMatrix(45.0f, 0.1f, 100.0f);
        camera.Matrix(shaderProgram, "camMatrix");

        // Object Loop
        for (int i = 0; i < objects.size(); i++) {
            Object currObject = objects[i];

            // Load mesh
            currObject.mesh->vao->Bind();

            // Load texture
            currObject.texture->Bind();

            // Load transformations
            modelMatrix = currObject.getModelMatrix();
            shaderProgram.setUniform("modelMatrix", modelMatrix);
            glDrawElements(GL_TRIANGLES, mesh.index_count, GL_UNSIGNED_INT, 0);
        }

        // Swap Front/Back Buffers
        glfwSwapBuffers(window);

        // Detect and handle events
        glfwPollEvents();

    }
    // --------------------------------------------------------------------------------------------------
    // APPLICATION CLEAN-UP

    shaderProgram.Delete();
    texture.Delete();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
// =======================================================================================================
