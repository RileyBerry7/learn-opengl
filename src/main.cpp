// main.cpp
// Created 3/5/26

#include <glad/glad.h>
#include <GLFW/glfw3.h>
// #include <GL/gl.h>
# include <stb/stb_image.h>

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
    -0.5f, -0.5f, 0.0f,   1.00f, 0.01f, 0.12f,  // Lower left
    -0.5f,  0.5f, 0.0f,   0.09f, 0.95f, 0.17f,  // Upper left
     0.5f,  0.5f, 0.0f,   0.90f, 0.45f, 0.70f,  // Lower right
     0.5f, -0.5f, 0.0f,   0.10f, 0.30f, 0.93f   // Upper right
};

GLuint indices[] = {
    0, 2, 1, // Upper triangle
    0, 3, 2  // Lower triangle
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
    VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);;
    VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3*sizeof(float)));;

    VAO1.Unbind();
    VBO1.Unbind();
    // EBO1.Unbind();

    GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");


    // Main Render Loop
    // --------------------------------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        /* render here */
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glUniform1f(uniID, 0.5f);
        VAO1.Bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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
