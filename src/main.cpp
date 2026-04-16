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


struct LightSource {
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    Object*   object;
};

// ------------------------------------------------------------------------------------------------------
// GLOBAL CONFIGURATION

constexpr int  WIDTH  = 800;
constexpr int  HEIGHT = 600;
constexpr char WINDOW_NAME[] = "Window";

// Background Color
glm::vec4  bgColor     = glm::vec4(0.07f, 0.13f, 0.17f, 1.0f);
// glm::vec4   bgColor     = glm::vec4(0.02f, 0.01f, 0.04f, 1.0f);

// Model Files
// std::string objectFile  = "ISD.obj";
// std::string textureFile = "ISD_hull_color_baked.png";
const std::string objectFile  = "utah_teapot.obj";
const std::string textureFile = "missing.png";

// Shader Files
const char vertexShaderFile[]   = "default.vert";
const char fragmentShaderFile[] = "default.frag";

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

    // Uniform Names
    const char* texUniform = "tex0";
    // const char* lightUniform = "lightUniform";

    // Uniform Values
    // glm::mat4 modelMatrix = glm::mat4(1.0f);
    // glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    // glm::vec3 lightPos = glm::vec3(1.0f, 0.15f, 0.2f);

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
    // LOAD SHADERS
    Shader defaultShader(vertexShaderFile, fragmentShaderFile);
    defaultShader.Activate();

    Shader emisiveShader(vertexShaderFile, "light_source.frag");
    emisiveShader.Activate();
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
    texture.setUniform(defaultShader, texUniform, 0);

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
    Object object1(defaultShader, mesh, texture);
    object1.rotation = glm::vec3(-90, 0.0f, 0);
    // object1.scale = glm::vec3(0.001f);
    object1.scale = glm::vec3(0.1f);
    objects.push_back(object1);

    // OBJECT 2 (CUBE)
    Mesh cube_mesh("resources/models/cube.obj");
    Tex  cube_texture("resources/textures/osaka.png", texType, texSlot, pixelType);
    // texture.setUniform(shaderProgram, texUniform, 0);
    Object object2(emisiveShader, cube_mesh, cube_texture);
    object2.position = glm::vec3(1.0f, 0.15f, 0.2f);
    object2.scale = glm::vec3(0.1f);
    objects.push_back(object2);

    //------------------------------------------------------------------------
    // LIGHT SOURCE LIST
    std::vector<LightSource> lights;

    // Light 1
    LightSource light1 = {
        .position = glm::vec3(1.0f, 0.15f, 0.2f),
        .ambient  = glm::vec3(0.2, 0.2, 0.2),
        .diffuse  = glm::vec3(0.5, 0.5, 0.5),
        .specular = glm::vec3(1.0, 1.0, 1.0),
        .object   = &object2
    };
    lights.push_back(light1);

    //===================================================================================================
    // Main Render Loop
    // --------------------------------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        // Experimentation
        // glm::vec3 lightColor2;
        // lightColor.x = sin(glfwGetTime() * 3.0f);
        // lightColor.y = sin(glfwGetTime() * 1.7f);
        // lightColor.z = sin(glfwGetTime() * 2.3f);
        // glm::vec3 diffuseColor = lightColor   * glm::vec3(0.7f);
        // glm::vec3 ambientColor = diffuseColor * glm::vec3(0.4f);

        // Replace background color
        glClearColor(bgColor.r, bgColor.g, bgColor.b, bgColor.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Camera Timing
        float currentTime = glfwGetTime();
        float deltaTime   = currentTime - lastTime;
        lastTime          = currentTime;
        camera.Inputs(window, deltaTime);

        // Camera Updating
        camera.UpdateMatrix(45.0f, 0.1f, 100.0f);

        // Set Camera uniforms
        defaultShader.Activate();
        camera.Matrix(defaultShader, "camMatrix");
        emisiveShader.Activate();
        camera.Matrix(emisiveShader, "camMatrix");

        // Set default uniforms
        defaultShader.Activate();
        defaultShader.setUniform("viewPos", camera.Position);

        // Light-source Loop
        for (int i = 0; i < lights.size(); i++) {
            LightSource* currLight = &lights[i];

            defaultShader.Activate();
            defaultShader.setUniform("light.position", currLight->position);
            defaultShader.setUniform("light.ambient",  currLight->ambient);
            defaultShader.setUniform("light.diffuse",  currLight->diffuse);
            defaultShader.setUniform("light.specular", currLight->specular);
        }

        // Object Loop
        for (int i = 0; i < objects.size(); i++) {
            Object currObject = objects[i];

            // Activate object's shader/mesh/texture
            currObject.shader->Activate();
            currObject.mesh->vao->Bind();
            currObject.texture->Bind();

            // Set object-specific uniforms
            defaultShader.setUniform("modelMatrix", currObject.getModelMatrix());

            // Draw Object
            glDrawElements(GL_TRIANGLES, mesh.index_count, GL_UNSIGNED_INT, 0);
        }

        // Swap Front/Back Buffers
        glfwSwapBuffers(window);

        // Detect and handle events
        glfwPollEvents();

    }
    // --------------------------------------------------------------------------------------------------
    // APPLICATION CLEAN-UP

    defaultShader.Delete();
    texture.Delete();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
// =======================================================================================================
