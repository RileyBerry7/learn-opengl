// main.cpp
// Created 3/5/26

#include <glad/glad.h>
#include <GLFW/glfw3.h>
// #include <GL/gl.h>

#include "shaderClass.h"
#include "vbo.h"
#include "ebo.h"
#include "vao.h"

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
    -0.5f,     -0.5f * float(sqrt(3))  / 3, 0.0f, // outer-left
     0.5f,     -0.5f * float(sqrt(3))  / 3, 0.0f, // outer-right
     0.0f,      0.5f * float(sqrt(3))*2/ 3, 0.0f,  // outer-top
    -0.5f / 2,  0.5f * float(sqrt(3))  / 6, 0.0f,  // inner-left
     0.5f / 2,  0.5f * float(sqrt(3))  / 6, 0.0f,  // inner-right
     0.0f,     -0.5f * float(sqrt(3))  / 3, 0.0f  // inner-btm
};

GLuint indices[] = {
    0, 3, 5, // lower-left triangle
    3, 2, 4, // lower-right triangle
    5, 4, 1  // upper triangle
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
    std::cout << "Hello OpenGL!";
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

    VAO1.LinkVBO(VBO1, 0);
    VAO1.Unbind();
    VBO1.Unbind();
    EBO1.Unbind();


    // Main Render Loop
    // --------------------------------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        /* render here */
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        VAO1.Bind();
        glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);
        glfwSwapBuffers(window);

        // Detect and Handle any GLFW events
        glfwPollEvents();

    } // --------------------------------------------------------------------------------------------------

    // Clean up vertex array, buffer and shader program
    VAO1.Delete();
    VBO1.Delete();
    EBO1.Delete();
    shaderProgram.Delete();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
// =======================================================================================================
