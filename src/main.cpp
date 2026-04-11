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
#include "model.h"

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

    class Object {
    public:
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3  scale;

        Object(Model mesh) {
            position = glm::vec3(0.0f, 0.0f, 0.0f);
            rotation = glm::vec3(0.0f, 0.0f, 0.0f);
            scale    = glm::vec3(1.0f, 1.0f, 1.0f);
        }

        glm::mat4 getModelMatrix() {
            glm::mat4 model = glm::mat4(1.0f); // Identity matrix
            model = glm::translate(model, position);
            model = glm::rotate(model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::scale(model, scale);
            return model;
        }
    };
    //----------------------------------------------------------------------------------------------------
    // LOAD MODEL
    std::string objFilePath = "resources/models/" + objectFile;
    Model model(objFilePath);

    // Vertex Count & Index Count
    std::cout << "\nVertex Count: " << model.vertex_count << std::endl;
    std::cout << "Index Count: "    << model.index_count  << std::endl;

    // Vertex & Index Data
    GLfloat* vertices = model.vertexBuffer.data();
    GLuint*  indices  = model.indexBuffer.data();

    //------------------------------------------------------------------------------------------------------
    // // LOAD LIGHT
    // Model light("resources/models/cube.obj");
    //
    // // Vertex & Index Data
    // GLfloat* light_vertices = light.vertexBuffer.data();
    // GLuint*  light_indices  = light.indexBuffer.data();
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

    // -------------------------------------------------------------------------------------
    // GRAPHICS PIPELINE SETUP

    Shader shaderProgram("default.vert", "default.frag");
    shaderProgram.Activate();

    VAO VAO1; // Model

    VAO1.Bind();
    VBO VBO1(vertices, sizeof(GLfloat) * model.vertex_count * model.stride); // 8 = stride length
    EBO EBO1(indices, sizeof(unsigned int)  * model.index_count);

    // Links VBO1 to VAO
    VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, model.stride * sizeof(float), (void*)0);;
    VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, model.stride * sizeof(float), (void*)(3*sizeof(float)));;
    VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, model.stride * sizeof(float), (void*)(6*sizeof(float)));;

    VAO1.Unbind();
    VBO1.Unbind();

    // -------------------------------------------------------------------------------------
    // TEXTURES

    int numColorCh;

    std::string filePath = "resources/textures/" + textureFile;
    GLenum texType = GL_TEXTURE_2D;
    GLenum texSlot = GL_TEXTURE0;
    GLenum pixelType = GL_UNSIGNED_BYTE;

    // Texture
    Tex texture = Tex(filePath.c_str(), texType, texSlot, pixelType);
    const char* texUniform = "tex0";
    constexpr GLuint unit = 0;
    texture.texUnit(shaderProgram, texUniform, unit);

    // TRANFORMATIONS -------------------

    // ...

    // DEPTH -----------------------------
    glEnable(GL_DEPTH_TEST);

    Camera camera(WIDTH, HEIGHT, glm::vec3(0.0f, 0.0f, 2.f));

    // Wireframe rendering
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    float lastTime = glfwGetTime();

    // OBJECTS
    std::vector<Object> objects;
    Object buffer1(model);
    objects.push_back(buffer1);

    Object buffer2(model);
    buffer2.position.x = 1.0f;
    buffer2.scale = glm::vec3(1.0f, 1.0f, 1.0f);
    // buffer2.rotation.x = glm::pi<float>() / 2.0f;
    objects.push_back(buffer2);

    // Model-Matrix Uniform
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
    // modelMatrix = buffer.getModelMatrix();


    //===================================================================================================
    // Main Render Loop
    // --------------------------------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(bgColor.r, bgColor.g, bgColor.b, bgColor.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaderProgram.Activate();

        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        camera.Inputs(window, deltaTime);

        // New camera
        camera.UpdateMatrix(45.0f, 0.1f, 100.0f);
        camera.Matrix(shaderProgram, "camMatrix");

        // Texture
        // glUniform1f(uniID, 0.5f);
        texture.Bind();

        VAO1.Bind();

        modelMatrix = buffer1.getModelMatrix();
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
        glDrawElements(GL_TRIANGLES, model.index_count, GL_UNSIGNED_INT, 0);

        modelMatrix = buffer2.getModelMatrix();
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
        glDrawElements(GL_TRIANGLES, model.index_count, GL_UNSIGNED_INT, 0);
        glfwSwapBuffers(window);

        // Detect and Handle any GLFW events
        glfwPollEvents();

    }
    // --------------------------------------------------------------------------------------------------
    // APPLICATION CLEAN-UP

    VAO1.Delete();
    VBO1.Delete();
    EBO1.Delete();
    shaderProgram.Delete();
    texture.Delete();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
// =======================================================================================================
