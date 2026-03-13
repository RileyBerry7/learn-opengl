// main.cpp
// Created 3/5/26

#include <glad/glad.h>
#include <GLFW/glfw3.h>
// #include <GL/gl.h>
# include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shaderClass.h"
#include "vbo.h"
#include "vao.h"
#include "ebo.h"

// std
#include <iostream>
#include <string>
#include <cmath>

// ------------------------------------------------------------------------------------------------------
// GLOBALS

constexpr int  WIDTH  = 800;
constexpr int  HEIGHT = 600;
constexpr char WINDOW_NAME[] = "Window";


// ----------------------------------------------------
// Vertex Data
GLfloat vertices[] = {

    //  COORDINATES  //   //     COLORS     //
    -0.5f, -0.5f, 0.5f,   1.0f, 0.00f, 0.0f,   0.0f, 0.0f, // Lower left
    -0.5f,  0.5f, 0.5f,   0.0f, 1.0f, 0.0f,    0.0f, 1.0f, // Upper left
     0.5f,  0.5f, 0.5f,   0.0f, 0.0f, 1.0f,    1.0f, 1.0f, // Lower right
     0.5f, -0.5f, 0.5f,   1.0f, 1.0f, 1.0f,    1.0f, 0.0f,  // Upper right

    -0.5f, -0.5f, -0.5f,   1.0f, 0.00f, 0.0f,   0.0f, 0.0f, // Lower left
    -0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,    0.0f, 1.0f, // Upper left
     0.5f,  0.5f, -0.5f,   0.0f, 0.0f, 1.0f,    1.0f, 1.0f, // Lower right
     0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,    1.0f, 0.0f  // Upper right
};

GLuint indices[] = {
    // Front
    0, 1, 2, // Upper triangle
    0, 2, 3,  // Lower triangle

    // Back
    4, 6, 5, // Upper triangle
    4, 7, 6,  // Lower triangle

    // Left
    0, 5, 1, // Upper triangle
    0, 4, 5,  // Lower triangle

    // Right
    3, 2, 6, // Upper triangle
    3, 6, 7,  // Lower triangle

    // Top
    1, 5, 6,  // Upper triangle
    1, 6, 2,  // Lower triangle

    // Bottom
    0, 7, 3, // Upper triangle
    0, 4, 7,  // Lower triangle
};

// ----------------------------------------------------
// Process Input
void processInput (GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)  {
        std::cout << "User pressed ESC\n";
        glfwSetWindowShouldClose(window, true);
    }
}

// =======================================================================================================
int main() {
    std::cout << "Hello OpenGL!\n";
    // GLFW Initialization
    if (!glfwInit()) return -1;

    // Specify OpenGL version for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // Specify OpenGL profile for GLFW
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Core: lacks deprecated functions

    // Create a GLFW window object
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, WINDOW_NAME, nullptr, nullptr);
    // Error check: Failed to create window
    if (!window) {
        glfwTerminate();
        return -1;
    }

    // Introduce window to the current context
    glfwMakeContextCurrent(window);

    // Load GLAD so it configures OpenGl to be driver agnostic
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }
    // Define viewport size - area in which OpenGL can render
    glViewport(0, 0, WIDTH, HEIGHT);

    // -------------------------------------------------------------------------------------
    // GRAPHICS PIPELINE SETUP
    Shader shaderProgram("default.vert", "default.frag");
    shaderProgram.Activate();

    VAO VAO1;
    VAO1.Bind();

    VBO VBO1(vertices, sizeof(vertices));
    EBO EBO1(indices, sizeof(indices));

    // Links VBO to VAO
    VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);;
    VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3*sizeof(float)));;
    VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6*sizeof(float)));;

    VAO1.Unbind();
    VBO1.Unbind();
    // EBO1.Unbind();

    GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");

    // Textures ---------------

    int widthImg, heightImg, numColorCh;
    stbi_set_flip_vertically_on_load(true);
    std::string fileName = "resources/textures/osaka.png";
    unsigned char* bytes = stbi_load(fileName.c_str(), &widthImg, &heightImg, &numColorCh, 0);
    if (!bytes) {
        std::cerr << "Failed to load texture: " + fileName + "\n";
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // float flatColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    // glParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, flatColor);
    GLenum format = (numColorCh == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, widthImg, heightImg, 0, format, GL_UNSIGNED_BYTE, bytes);

    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(bytes);
    glBindTexture(GL_TEXTURE_2D, 0);

    GLuint tex0Uni = glGetUniformLocation(shaderProgram.ID, "tex0");
    shaderProgram.Activate();
    glUniform1i(tex0Uni, 0);

    // 3D transformations
    float rotation = 0.0f;
    double prevTime = glfwGetTime();

    // DEPTH -----------------------------
    glEnable(GL_DEPTH_TEST);


    // Main Render Loop
    // --------------------------------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        /* render here */
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaderProgram.Activate();

        double crnTime = glfwGetTime();
        if (crnTime - prevTime >= 1.0f / 60.0f) {
            rotation += 0.5f;
            prevTime = crnTime;
        }

        glm::mat4 model = glm::mat4(1.0f); // Identity Matrix
        glm::mat4 view = glm::mat4(1.0f); // Identity Matrix
        glm::mat4 proj = glm::mat4(1.0f); // Identity Matrix

        model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
        view = glm::translate(view, glm::vec3(0.0f, -0.5f, -2.0f));
        proj = glm::perspective(glm::radians(45.0f), (float)(800/800), 0.1f, 100.0f);

        int modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        int viewLoc = glGetUniformLocation(shaderProgram.ID, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(model));
        int projLoc = glGetUniformLocation(shaderProgram.ID, "proj");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(model));

        glUniform1f(uniID, 0.5f);
        glBindTexture(GL_TEXTURE_2D, texture);
        VAO1.Bind();
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glfwSwapBuffers(window);

        // Detect and Handle any GLFW events
        glfwPollEvents();

    } // --------------------------------------------------------------------------------------------------

    // Clean up vertex array, buffer and shader program
    VAO1.Delete();
    VBO1.Delete();
    EBO1.Delete();
    shaderProgram.Delete();
    glDeleteTextures(1, &texture);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
// =======================================================================================================
