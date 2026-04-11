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

#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "tiny_obj_loader.h"



// Vertex struct
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
};

// ------------------------------------------------------------------------------------------------------
// GLOBALS

constexpr int  WIDTH  = 800;
constexpr int  HEIGHT = 600;
constexpr char WINDOW_NAME[] = "Window";

//glm::vec4 bgColor = glm::vec4(0.07f, 0.13f, 0.17f, 1.0f);
glm::vec4 bgColor = glm::vec4(0.02f, 0.01f, 0.04f, 1.0f);
std::string objectFile  = "ISD.obj";
std::string textureFile = "ISD_hull_color_baked.png";


// ----------------------------------------------------
// Process Input
void processInput (GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)  {
        std::cout << "User pressed ESC\n";
        glfwSetWindowShouldClose(window, true);
    }
}

//
GLfloat lightVertices[] =
    { // Coordinates

};

// =======================================================================================================
int main() {

    // Parse obj file
    // char obj_file_path[] = "resources/models/stanford_teapot.obj";
    // ObjParse parse_output = parse(obj_file_path);



    // ----------------------------------------------------
    // Vertex Data
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;
    std::string filename = "resources/models/" + objectFile;

    bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename.c_str());
    if (!warn.empty()) std::cout << "WARN: " << warn << "\n";
    if (!err.empty())  std::cout << "ERR: " << err << "\n";
    if (!success) {
        std::cout << "Failed to load OBJ\n";
        return 1;
    }
    std::vector<float> vertexBuffer;
    vertexBuffer.reserve(shapes.size() * 1000); // optional optimization

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {

            // -------------------------
            // POSITION (x, y, z)
            // -------------------------
            float vx = attrib.vertices[3 * index.vertex_index + 0];
            float vy = attrib.vertices[3 * index.vertex_index + 1];
            float vz = attrib.vertices[3 * index.vertex_index + 2];

            // -------------------------
            // COLOR (r, g, b)
            // OBJ usually doesn't have this → default white
            // -------------------------
            float r = 1.0f;
            float g = 1.0f;
            float b = 1.0f;

            // -------------------------
            // UV (u, v)
            // -------------------------
            float u = 0.0f;
            float v = 0.0f;

            if (!attrib.texcoords.empty() && index.texcoord_index >= 0) {
                u = attrib.texcoords[2 * index.texcoord_index + 0];
                v = attrib.texcoords[2 * index.texcoord_index + 1];
            }

            // -------------------------
            // PUSH INTERLEAVED VERTEX
            // -------------------------
            vertexBuffer.push_back(vx);
            vertexBuffer.push_back(vy);
            vertexBuffer.push_back(vz);

            vertexBuffer.push_back(r);
            vertexBuffer.push_back(g);
            vertexBuffer.push_back(b);

            vertexBuffer.push_back(u);
            vertexBuffer.push_back(v);
        }
    }
    //------------------------------------------------------
    std::vector<unsigned int> out_indices;

    for (unsigned int i = 0; i < vertexBuffer.size()/8; i++) {
        out_indices.push_back(i);
    }
    //------------------------------------------------------

    const int VERTICES_COUNT = vertexBuffer.size() / 8;
    const int INDICES_COUNT  = out_indices.size();

    std:: cout << std::endl << std::endl;
    std::cout << "Vertex Count: " << VERTICES_COUNT << std::endl;
    std::cout << "Index Count: "  << INDICES_COUNT  << std::endl;

    GLfloat* vertices = vertexBuffer.data();
    GLuint*  indices  = out_indices.data();


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

    VBO VBO1(vertices, sizeof(GLfloat) * VERTICES_COUNT * 8); // 8 = stride length
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
