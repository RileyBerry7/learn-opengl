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
    auto emisiveShader = Shader("default.vert", "light_source.frag"); // Initialize emissive shader
    float lastTime     = glfwGetTime(); // Initialize Timer

    // Load meshes
    std::string     path = "resources/models/";
    Mesh cubeMesh  (path + "cube.obj");
    Mesh sphereMesh(path + "sphere.obj");

    // Load textures
    path          = "resources/textures/";
    GLenum tt     = GL_TEXTURE_2D;
    GLenum pt     = GL_UNSIGNED_BYTE;
    auto texture  = Tex(path + "wood_crate.png"  , tt, GL_TEXTURE0, pt); // Primary texture
    auto texture2 = Tex(path + "crate_border.png", tt, GL_TEXTURE1, pt); // Specular map

    // Load materials
    auto steel         = DefaultMaterial(defaultShader, &texture, &texture2);
    auto lightMaterial = EmissiveMaterial(emisiveShader);


    // ------------------------- Initialize lights -------------------------

    //     glm::vec3 direction = glm::vec3(-0.2f, -0.0f, -0.1f);

    auto light0 = PointLight(glm::vec3(3.3f, 0.5f, 0.7f), 1.0f, 0.09f, 0.032f,
        glm::vec3(0.3f, 0.3f, 0.3f),
        glm::vec3(0.5f, 0.5f, 0.5f),
        glm::vec3(1.0f, 1.0f, 1.0f));
    auto light1 = PointLight(light0);
    light1.position = glm::vec3(-1.0f, 0.0f, 0.2f);
    auto light2 = SpotLight(glm::vec3(0.0f), glm::vec3(0.0f), std::cos(std::numbers::pi/15.0f),
                           glm::vec3(0.3f, 0.3f, 0.3f),
                            glm::vec3(0.5f, 0.5f, 0.5f),
                            glm::vec3(1.0f, 1.0f, 1.0f));

    std::vector<std::unique_ptr<Light>> lights;
    lights.push_back(std::make_unique<PointLight>(light0));
    lights.push_back(std::make_unique<PointLight>(light1));
    lights.push_back(std::make_unique<SpotLight>(light2));

    // ------------------------- Initialize objects -------------------------

    Object object0(defaultShader, cubeMesh, texture,steel );
    object0.rotation = glm::vec3(0.0f, -44.0f, 0.0f);

    Object object1(defaultShader, cubeMesh, texture, steel);
    object1.position += glm::vec3(1.8f, 0.3f, -1.3f);object1.rotation.z += 10;

    Object object2(defaultShader, cubeMesh, texture, steel);
    object2.position = glm::vec3(5.0f, 0.7f, 0.3f);object2.rotation.x += 8;object2.rotation.z += 15;

    Object object3(defaultShader, cubeMesh, texture, steel);
    object3.position = glm::vec3(3.0f, 0.1f, -1.0f);

    Object object4(emisiveShader, sphereMesh, texture, lightMaterial);
    object4.position = glm::vec3(3.3f, 0.5f, 0.7f); object4.scale = glm::vec3(0.4);

    Object object5(emisiveShader, sphereMesh, texture, lightMaterial);
    object5.position = light1.position;object5.scale = glm::vec3(0.4);

    std::vector<Object> objects;
    objects.push_back(object0);
    objects.push_back(object1);
    objects.push_back(object2);
    objects.push_back(object3);
    objects.push_back(object4);
    objects.push_back(object5);

    //===================================================================================================
    // Render Loop
    // --------------------------------------------------------------------------------------------------
    while (!window.shouldClose()) {

        window.processInput();

        // Update Flashlight
        light2.direction = glm::normalize(camera.Orientation);
        light2.position  = camera.Position;
        lights[2]        = std::make_unique<SpotLight>(light2);

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
