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
#include  "material.h"
#include  "window.h"
#include  "renderer.h"
#include  "light.h"

// std
#include <iostream>
#include <string>

// =====================================================================================================================
int main() {

    std::cout << "\nHello OpenGL!\n";

    auto window   = Window();   // Initialize GLFW window
    auto renderer = Renderer(); // Initialize renderer
    auto camera   = Camera(window.width, window.height, glm::vec3(2.3f, 0.0f, 7.0f)); // Initialize camera
    auto defaultShader = Shader("default.vert", "default.frag");      // Initialize default shader
    auto emisiveShader = Shader("default.vert", "emissive.frag");     // Initialize emissive shader
    float lastTime     = glfwGetTime(); // Initialize Timer

    // Mesh Map
    std::map<std::string, std::unique_ptr<Mesh>> meshMap;
    meshMap["cube.obj"]       = std::make_unique<Mesh>("cube.obj"  , defaultShader);
    meshMap["sphere.obj"]     = std::make_unique<Mesh>("sphere.obj", emisiveShader);
    meshMap["Floor.obj"]      = std::make_unique<Mesh>("Floor.obj" , defaultShader);

    // ------------------------- Initialize lights -------------------------
    auto light0 = PointLight {};
    light0.position  = glm::vec3(3.3f, 0.5f, 0.7f);
    light0.intensity = 0.5f;
    light0.color     = glm::vec3(1.0f);
    light0.constant  = 1.0f;
    light0.linear    = 0.09f;
    light0.quadratic = 0.032f;
    light0.radius    = 1.0f;
    auto light1 = light0;
    light1.position = glm::vec3(-1.0f, 0.0f, 0.2f);
    auto light2 = SpotLight {};
    light2.position    = glm::vec3(0.0f);
    light2.intensity   = 0.8f;
    light2.direction   = glm::vec3(0.0f);
    light2.cutOff      = std::cos(std::numbers::pi/17.0f);
    light2.color       = glm::vec3(1.0f);
    light2.outerCutOff = std::cos(std::numbers::pi/13);

    LightManager lights(defaultShader);
    lights.pointBucket.push_back(light0);
    lights.pointBucket.push_back(light1);
    lights.spotBucket.push_back(light2);

    // ------------------------- Initialize objects -------------------------

    Object object0(defaultShader, *meshMap["cube.obj"].get());
    Object object1(object0);
    Object object2(object0);
    Object object3(object0);
    Object object4(emisiveShader, *meshMap["sphere.obj"].get());
    Object object5(object4);
    Object object6(defaultShader, *meshMap["Floor.obj"].get());
    object0.rotation = glm::vec3(0.0f, -44.0f, 0.0f);
    object1.position += glm::vec3(1.8f, 0.3f, -1.3f);object1.rotation.z += 10;
    object2.position = glm::vec3(5.0f, 0.7f, 0.3f);object2.rotation.x += 8;object2.rotation.z += 15;
    object3.position = glm::vec3(3.0f, 0.1f, -1.0f);
    object4.position = glm::vec3(3.3f, 0.5f, 0.7f); object4.scale = glm::vec3(0.4);
    object5.position = light1.position;object5.scale = glm::vec3(0.4);
    object6.position = glm::vec3(0.0f, -1.0f, 0.0f);
    object6.scale    = glm::vec3(0.1);

    std::vector<Object> objects;
    objects.push_back(object0);
    objects.push_back(object1);
    objects.push_back(object2);
    objects.push_back(object3);
    objects.push_back(object4);
    objects.push_back(object5);
    objects.push_back(object6);

    //===================================================================================================
    // Render Loop
    // --------------------------------------------------------------------------------------------------
    while (!window.shouldClose()) {

        window.processInput();

        // Update Flashlight
        light2.direction = glm::normalize(camera.Orientation);
        light2.position  = camera.Position;
        lights.spotBucket.pop_back();
        lights.spotBucket.push_back(light2);

        renderer.renderScene(objects, lights, camera, defaultShader);

        camera.Inputs(window.getWindow(), glfwGetTime() - lastTime);
        camera.UpdateMatrix(45.0f, 0.1f, 100.0f);
        lastTime = static_cast<float>(glfwGetTime());

        window.swapBuffers();
    }
    // --------------------------------------------------------------------------------------------------

    return 0;
}
// =======================================================================================================
