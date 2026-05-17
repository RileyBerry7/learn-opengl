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
unsigned int loadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;

    // IMPORTANT: Cubemaps usually should NOT be flipped vertically,
    // unlike 2D textures, as it messes up the face orientations.
    stbi_set_flip_vertically_on_load(false);

    for (unsigned int i = 0; i < faces.size(); i++)
    {
        // Force 4 channels (RGBA) to ensure 4-byte memory alignment
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 4);

        if (data)
        {
            // Use GL_RGBA for both internalFormat and format to match the '4' above
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }

    // Set essential filtering and wrapping
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Use GL_CLAMP_TO_EDGE to prevent visible seams between faces
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

// =====================================================================================================================
int main() {

    std::cout << "\nHello OpenGL!\n";

    auto window   = Window();   // Initialize GLFW window
    auto renderer = Renderer(); // Initialize renderer
    auto camera   = Camera(window.width, window.height, glm::vec3(2.3f, 0.0f, 7.0f)); // Initialize camera
    auto defaultShader = Shader("default.vert", "default.frag");      // Initialize default shader
    auto emisiveShader = Shader("default.vert", "emissive.frag");     // Initialize emissive shader
    auto skyboxShader  = Shader("skybox.vert", "skybox.frag");
    auto shadowShader  = Shader("shadow.vert", "shadow.frag");
    float lastTime     = glfwGetTime(); // Initialize Timer

    // Shader Map
    std::map<std::string, Shader*> shaderMap;
    shaderMap["default"]  = &defaultShader;
    shaderMap["emissive"] = &emisiveShader;
    shaderMap["skybox"]   = &skyboxShader;
    shaderMap["shadow"]   = &shadowShader;

    // Mesh Map
    std::map<std::string, std::unique_ptr<Mesh>> meshMap;
    meshMap["cube.obj"]       = std::make_unique<Mesh>("cube.obj",   shaderMap);
    meshMap["sphere.obj"]     = std::make_unique<Mesh>("sphere.obj", shaderMap);
    meshMap["Floor.obj"]      = std::make_unique<Mesh>("Floor.obj",  shaderMap);
    auto skybox = Mesh("cube.obj", shaderMap);

    // Create texture atlas
    std::vector<std::string> cubemap_faces;
    cubemap_faces.push_back("resources/cubemap/right.jpg");
    cubemap_faces.push_back("resources/cubemap/left.jpg");
    cubemap_faces.push_back("resources/cubemap/top.jpg");
    cubemap_faces.push_back("resources/cubemap/bottom.jpg");
    cubemap_faces.push_back("resources/cubemap/front.jpg");
    cubemap_faces.push_back("resources/cubemap/back.jpg");

    unsigned int cubemapTexture = loadCubemap(cubemap_faces);

    // ------------------------- Initialize lights -------------------------
    auto light0 = PointLight {};
    light0.position  = glm::vec3(3.3f, 0.5f, 0.7f);
    light0.intensity = 0.5f;
    // Disgusting please remove
    light0.color     = static_cast<EmissiveMaterial*>(meshMap["sphere.obj"]->materialList[1].get())->lightColor;
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

    auto light3 = DirLight{};
    light3.color = glm::vec3(1.0f);
    light3.direction = glm::vec3(0.4f, -10.0f, -3.0f);
    light3.intensity = 0.05f;

    LightManager lights(defaultShader);
    lights.pointBucket.push_back(light0);
    lights.pointBucket.push_back(light1);
    lights.spotBucket.push_back(light2);
    lights.dirBucket.push_back(light3);

    // ------------------------- Initialize objects -------------------------

    Object object0(defaultShader, *meshMap["cube.obj"].get());
    Object object1(object0);
    Object object2(object0);
    Object object3(object0);
    auto   object7 = Object(object0);
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
    object7.position = glm::vec3(7.5f, 0.5f, 0.7f);


    std::vector<Object> objects;
    objects.push_back(object0);
    objects.push_back(object1);
    objects.push_back(object2);
    objects.push_back(object3);
    objects.push_back(object4);
    objects.push_back(object5);
    objects.push_back(object6);
    objects.push_back(object7);

    // Create Frame Buffer
    GLuint depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    // next create a 2D texture we'll use as the framebuffer's depth buffer
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

    GLuint depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH,
    SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // With the generated depth texture we can attach it to the framebuffer's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // With a properly configured framebuffer, we can start the first pass


    //===================================================================================================
    // Render Loop
    // --------------------------------------------------------------------------------------------------
    while (!window.shouldClose()) {

        window.processInput();

        // Shadow map
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        // configure shadow map shader / matrices
        // RenderScene();
        // renderer.renderScene(objects, lights, camera, *shadowShader); // my regular renderScene function
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glViewport(0, 0, 800, 600); // reset viewport size
        // 2. Render scene as normal

        // Update Flashlight
        light2.direction = glm::normalize(camera.Orientation);
        light2.position  = camera.Position;
        lights.spotBucket.pop_back();
        lights.spotBucket.push_back(light2);

        defaultShader.Activate();
        defaultShader.setUniform("toggleF", window.f_toggle);

        renderer.renderScene(objects, lights, camera);

        // Skybox
        glDepthFunc(GL_LEQUAL); // Allow drawing at depth 1.0
        skyboxShader.Activate();
        skybox.vao->Bind();
        glm::mat4 view = glm::mat4(glm::mat3(camera.view)); // Strip movement!
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        skyboxShader.setUniform("cubemap", 0);
        skyboxShader.setUniform("projection", camera.projection);
        skyboxShader.setUniform("view", view);
        glDrawElements(GL_TRIANGLES, skybox.index_count,GL_UNSIGNED_INT, 0);
        glDepthFunc(GL_LESS); // Reset depth

        camera.Inputs(window.getWindow(), glfwGetTime() - lastTime);
        camera.UpdateMatrix(45.0f, 0.1f, 100.0f);
        lastTime = static_cast<float>(glfwGetTime());

        window.swapBuffers();
    }
    // --------------------------------------------------------------------------------------------------

    return 0;
}
// =======================================================================================================
