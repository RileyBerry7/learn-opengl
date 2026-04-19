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
    glm::vec3 direction;
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
const std::string objectFile  = "cube.obj";
const std::string textureFile = "wood_crate.png";

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

    // Specular texture map
    Tex texture2 = Tex("resources/textures/crate_border.png", texType, GL_TEXTURE1, pixelType);
    // texture2.setUniform(defaultShader, texUniform, 1);

    //-----------------------------------------------------------------------------
    // DEPTH
    glEnable(GL_DEPTH_TEST);
    Camera camera(WIDTH, HEIGHT, glm::vec3(0.0f, 0.0f, 2.f));

    //-----------------------------------------------------------------------------
    // WIREFRAME RENDERING
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    float lastTime = glfwGetTime();


    //------------------------------------------------------------------------
    // LIGHT SOURCE LIST
    std::vector<LightSource> lights;

    // Light 1
    LightSource light1 = {
        .position = glm::vec3(3.3f, 0.5f, 0.7f),
        .direction = glm::vec3(-0.2f, -0.0f, -0.1f),
        .ambient  = glm::vec3(0.3, 0.3, 0.3),
        .diffuse  = glm::vec3(0.5, 0.5, 0.5),
        .specular = glm::vec3(1.0, 1.0, 1.0),
    };
    lights.push_back(light1);


    //------------------------------------------------------------------------
    // OBJECT LIST
    std::vector<Object> objects;

    // Object 0 (Subject)
    Object object0(defaultShader, mesh, texture);
    object0.rotation = glm::vec3(0.0f, -44.0f, 0.0f);
    // object1.scale = glm::vec3(0.001f); // ISD
    objects.push_back(object0);

    Object object1(defaultShader, mesh, texture);
    object1.position += glm::vec3(1.8f, 0.3f, -1.3f);
    object1.rotation.z += 10;
    objects.push_back(object1);

    Object object2(defaultShader, mesh, texture);
    object2.position = glm::vec3(5.0f, 0.7f, 0.3f);
    object2.rotation.x += 8;
    object2.rotation.z += 15;
    objects.push_back(object2);

    Object object3(defaultShader, mesh, texture);
    object3.position = glm::vec3(3.0f, 0.1f, -1.0f);
    objects.push_back(object3);

    // Object 4 (Light Source)
    Mesh sphere_mesh("resources/models/sphere.obj");
    Object object4(emisiveShader, sphere_mesh, texture);
    object4.position = light1.position;
    object4.scale = glm::vec3(0.4);
    objects.push_back(object4);

    //------------------------------------------------------------------------
    // MATERIALS
    auto bronze  = Material(glm::vec3(1.0f, 0.5f, 0.31f), glm::vec3(1.0f, 0.5f, 0.31f), glm::vec3(0.5f, 0.5f, 0.5f), 32.0f);
    auto steel   = Material(glm::vec3(0.25f, 0.25f, 0.25f), glm::vec3(0.4f, 0.4f, 0.4f), glm::vec3(0.77f, 0.77f, 0.77f), 76.8f);
    auto wood    = Material(glm::vec3(0.1f, 0.07f, 0.05f), glm::vec3(0.4f, 0.25f, 0.15f), glm::vec3(0.1f, 0.1f, 0.1f), 10.0f);
    auto ceramic = Material(glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.1f, 0.5f, 0.8f), glm::vec3(1.0f, 1.0f, 1.0f), 128.0f);
    objects[0].material = steel;
    objects[1].material = steel;
    objects[2].material = steel;
    objects[3].material = steel;

    //===================================================================================================
    // Main Render Loop
    // --------------------------------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        // Experimentation
            auto sinColor = glm::vec3(sin(glfwGetTime() * 3.0f),
                                      sin(glfwGetTime() * 1.7f),
                                      sin(glfwGetTime() * 2.3f));

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
        for (auto &currLight : lights) {

            // Experimentation Continued
            // if (int(glfwGetTime())%5 == 0) {
            //     currLight.ambient = sinColor;
            //     currLight.diffuse = sinColor * 0.65f;
            // }

            objects[3].position.z -= 0.0025f;
            if (objects[3].position.z < -10.0f) objects[3].position.z = -0.15f;

            defaultShader.Activate();
            defaultShader.setUniform("light.position", currLight.position);
            defaultShader.setUniform("light.direction", currLight.direction);
            defaultShader.setUniform("light.ambient",  currLight.ambient);
            defaultShader.setUniform("light.diffuse",  currLight.diffuse);
            defaultShader.setUniform("light.specular", currLight.specular);

            defaultShader.setUniform("light.constant", 1.0f);
            defaultShader.setUniform("light.linear", 0.09f);
            defaultShader.setUniform("light.quadratic", 0.032f);

            emisiveShader.Activate();
            float brightness  = 5.0f;
            emisiveShader.setUniform("lightColor", (currLight.diffuse * brightness));
        }

        // Object Loop
        for (auto &currObject : objects) {

            // Activate object's shader/mesh/texture
            currObject.shader->Activate();
            currObject.mesh->vao->Bind();

            // Set object-specific uniforms
            currObject.shader->setUniform("modelMatrix", currObject.getModelMatrix());
            currObject.shader->setUniform("material.ambient", currObject.material.ambient);
            currObject.shader->setUniform("material.shininess", currObject.material.shininess);

            glActiveTexture(GL_TEXTURE0);
            currObject.texture->Bind();
            currObject.texture->setUniform(*currObject.shader, "material.diffuse", 0);

            glActiveTexture(GL_TEXTURE1);
            texture2.Bind();
            texture2.setUniform(*currObject.shader, "material.specular", 1);

            // Draw Object
            glDrawElements(GL_TRIANGLES, currObject.mesh->index_count, GL_UNSIGNED_INT, 0);
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
