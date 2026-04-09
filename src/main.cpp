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

// std
#include <iostream>
#include <string>
#include <cmath>

// C
extern "C" {
    #include "obj_parser.h"
}

#include "OBJ_Loader.h"

// ------------------------------------------------------------------------------------------------------
// GLOBALS

constexpr int  WIDTH  = 800;
constexpr int  HEIGHT = 600;
constexpr char WINDOW_NAME[] = "Window";

// ----------------------------------------------------
// Process Input
void processInput (GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)  {
        std::cout << "User pressed ESC\n";
        glfwSetWindowShouldClose(window, true);
    }
}

//

// =======================================================================================================
int main() {

    // Parse obj file
    char obj_file_path[] = "resources/models/sphere.obj";
    ObjParse parse_output = parse(obj_file_path);
    const int VERTICES_COUNT = parse_output.vertices_count;
    const int INDICES_COUNT  = parse_output.indices_count;



    // ----------------------------------------------------
    // Vertex Data

    GLfloat* vertices = reinterpret_cast<GLfloat*>(parse_output.vertices);
    GLuint*  indices   = reinterpret_cast<GLuint*>(parse_output.indices);



    std::cout << "\nHello OpenGL!\n";

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

    VBO VBO1(vertices, sizeof(GLfloat) * VERTICES_COUNT);
    EBO EBO1(indices, sizeof(unsigned int)  * INDICES_COUNT);

    // Links VBO to VAO
    VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);;
    VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3*sizeof(float)));;
    VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6*sizeof(float)));;

    VAO1.Unbind();
    VBO1.Unbind();
    // EBO1.Unbind(); // DO NOT UNBIND!!!


    // Textures ---------------

    int numColorCh;

    std::string filePath = "resources/textures/yuuri.png";
    GLenum texType = GL_TEXTURE_2D;
    GLenum texSlot = GL_TEXTURE0;
    GLenum pixelType = GL_UNSIGNED_BYTE;

    // Texture
    Tex texture = Tex(filePath.c_str(), texType, texSlot, pixelType);
    const char* texUniform = "tex0";
    constexpr GLuint unit = 0;
    texture.texUnit(shaderProgram, texUniform, unit);

    // TRANFORMATIONS -------------------


    // DEPTH -----------------------------
    glEnable(GL_DEPTH_TEST);

    Camera camera(WIDTH, HEIGHT, glm::vec3(0.0f, 0.0f, 2.f));

    // Wireframe rendering
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    float lastTime = glfwGetTime();

    // Main Render Loop
    // --------------------------------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        /* render here */
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaderProgram.Activate();


        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        camera.Inputs(window, deltaTime);

        // New camera
        camera.Matrix(45.0f, 0.1f, 100.0f, shaderProgram, "camMatrix");


        // Texture
        // glUniform1f(uniID, 0.5f);
        texture.Bind();

        VAO1.Bind();
        glDrawElements(GL_TRIANGLES, INDICES_COUNT, GL_UNSIGNED_INT, 0);
        glfwSwapBuffers(window);

        // Detect and Handle any GLFW events
        glfwPollEvents();

    } // --------------------------------------------------------------------------------------------------

    // Clean up vertex array, buffer and shader program
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
