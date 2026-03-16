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

// std
#include <iostream>
#include <string>
#include <cmath>

// C
extern "C" {
    #include "obj_parser.h"
}

// ------------------------------------------------------------------------------------------------------
// GLOBALS

constexpr int  WIDTH  = 800;
constexpr int  HEIGHT = 600;
constexpr char WINDOW_NAME[] = "Window";

// Parse obj file
char obj_file_path[] = "resources/models/cube.obj";
ObjParse parse_output = parse(obj_file_path);

const int VERTICES_COUNT = parse_output.vertices_count;
const int INDICES_COUNT  = parse_output.indices_count;


// ----------------------------------------------------
// Vertex Data

GLfloat* vertices = reinterpret_cast<GLfloat*>(parse_output.vertices);
GLuint* indices  = reinterpret_cast<GLuint*>(parse_output.indices);

// GLfloat vertices[] =
//     {
//
//     //  COORDINATES  //   //     COLORS        // UV COORDINATES
//     // Front
//     -0.5f, -0.5f,  0.5f,   1.0f, 0.00f, 0.0f,   0.0f, 0.0f, // Lower left
//     -0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f, // Upper left
//      0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f, // Upper right
//      0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f, // Lower right
//
//     // Back
//     -0.5f, -0.5f, -0.5f,   1.0f, 0.00f, 0.0f,   0.0f, 0.0f, // Lower left
//     -0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f, // Upper left
//      0.5f,  0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f, // Upper right
//      0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f, // Lower right
//
//     // Left
//     -0.5f, -0.5f, -0.5f,   1.0f, 0.00f, 0.0f,   0.0f, 0.0f, // Lower left
//     -0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f, // Upper left
//     -0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f, // Upper right
//     -0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f, // Lower right
//
//     // Right
//      0.5f, -0.5f, -0.5f,   1.0f, 0.00f, 0.0f,   0.0f, 0.0f, // Lower left
//      0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f, // Upper left
//      0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f, // Upper right
//      0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f, // Lower right
//
//     // Top
//     -0.5f,  0.5f,  0.5f,   1.0f, 0.00f, 0.0f,   0.0f, 0.0f, // Lower left
//     -0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f, // Upper left
//      0.5f,  0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f, // Upper right
//      0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f, // Lower right
//
//     // Bottom
//     -0.5f, -0.5f,  0.5f,   1.0f, 0.00f, 0.0f,   0.0f, 0.0f, // Lower left
//     -0.5f, -0.5f, -0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f, // Upper left
//      0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f, // Upper right
//      0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f  // Lower right
// };

// GLuint indices[] =
//     {
//     // Front
//     0, 1, 2, // Upper triangle
//     0, 2, 3,  // Lower triangle
//
//     // Back
//     4, 5, 6, // Upper triangle
//     4, 6, 7,  // Lower triangle
//
//     // Left
//     8, 9, 10, // Upper triangle
//     8, 10, 11,  // Lower triangle
//
//     // Right
//     12, 13, 14, // Upper triangle
//     12, 14, 15,  // Lower triangle
//
//     // Top
//     16, 17, 18,  // Upper triangle
//     16, 18, 19,  // Lower triangle
//
//     // Bottom
//     20, 21, 22, // Upper triangle
//     20, 22, 23  // Lower triangle
// };

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

    VBO VBO1(vertices, sizeof(GLfloat) * VERTICES_COUNT);
    EBO EBO1(indices, sizeof(unsigned int)  * INDICES_COUNT);

    // Links VBO to VAO
    VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);;
    VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3*sizeof(float)));;
    VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6*sizeof(float)));;

    VAO1.Unbind();
    VBO1.Unbind();
    // EBO1.Unbind(); // DO NOT UNBIND!!!

    GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");

    // Textures ---------------

    int numColorCh;

    std::string filePath = "resources/textures/yuuri.png";
    GLenum texType = GL_TEXTURE_2D;
    GLenum texSlot = GL_TEXTURE0;
    GLenum pixelType = GL_UNSIGNED_BYTE;

    // Instantiate Object
    Tex texture = Tex(filePath.c_str(), texType, texSlot, pixelType);

    // Tex Unit
    const char* texUniform = "tex0";
    constexpr GLuint unit = 0;
    texture.texUnit(shaderProgram, texUniform, unit);

    // 3D TRANSFORMATIONS ------------------------------
    float rotation = 0.0f;
    double prevTime = glfwGetTime();

    // DEPTH -----------------------------
    glEnable(GL_DEPTH_TEST);

    // Wireframe rendering
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Flip face direction
    // glFrontFace(GL_CW);

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

        // Rotate Towards and Down
        model = glm::translate(model, glm::vec3(0.0f, -0.3f, 0.5f));
        // model = glm::rotate(model, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.1f));

        model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.3f, 1.06, 0.7f));
        view = glm::translate(view, glm::vec3(0.0f, 0.3f, -6.0f));
        // view = glm::rotate(view, glm::radians(30.0f), glm::vec3(0.0f, -0.5, 0.5f));
        proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

        int modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        int viewLoc = glGetUniformLocation(shaderProgram.ID, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        int projLoc = glGetUniformLocation(shaderProgram.ID, "proj");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

        // Texture
        glUniform1f(uniID, 0.5f);
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
