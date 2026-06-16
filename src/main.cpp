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
#include "renderContext.h"

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

    std::cout << "\nHello OpenGL!" << std::endl;
    std::cout << std::string(100, '=') << std::endl << std::endl;

    std::map<std::string, Shader*> shaderMap; // TODO: Abstract all shaders into renderer (private)

    auto window   = Window();   // Initialize GLFW window
    auto renderer = Renderer(shaderMap); // Initialize renderer
    auto camera   = Camera(window.width, window.height, glm::vec3(2.3f, 0.0f, 7.0f)); // Initialize camera
    auto defaultShader    = Shader("default.vert", "default.frag");      // Initialize default shader
    auto emissiveShader   = Shader("default.vert", "emissive.frag");     // Initialize emissive shader
    auto skyboxShader     = Shader("skybox.glsl");
    auto shadow2dShader   = Shader("shadow2d.glsl");
    auto shadowCubeShader = Shader("shadowCube.glsl");


    // Shader Map
    shaderMap["default"]    = &defaultShader;
    shaderMap["emissive"]   = &emissiveShader;
    shaderMap["skybox"]     = &skyboxShader;
    shaderMap["shadow2d"]   = &shadow2dShader;
    shaderMap["shadowCube"] = &shadowCubeShader;
    auto copyMap = shaderMap;
    renderer.shaderMap = std::move(copyMap);  // TODO: remove when shaders are pushed into renderer
    std::cout << std::string(100, '=') << std::endl << std::endl;

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
    light0.position  = glm::vec3(3.3f, 0.5f, 0.9f);
    light0.intensity = 0.45f;
    // Disgusting please remove
    light0.color     = static_cast<EmissiveMaterial*>(meshMap["sphere.obj"]->materialList[1].get())->lightColor;
    light0.constant  = 1.0f;
    light0.linear    = 0.09f;
    light0.quadratic = 0.032f;
    light0.radius    = 25.0f;
    auto light1 = light0;
    light1.position = glm::vec3(-2.0f, 0.0f, 0.2f);
    auto light2 = SpotLight {}; // Flashlight
    light2.position    = glm::vec3(0.0f);
    light2.intensity   = 0.8f;
    light2.direction   = glm::vec3(0.0f);
    light2.cutOff      = std::cos(std::numbers::pi/17.0f);
    light2.color       = glm::vec3(1.0f);
    light2.outerCutOff = std::cos(std::numbers::pi/13);
    auto light3 = DirLight{}; // Moon
    light3.color = glm::vec3(1.0f);
    light3.direction = glm::vec3(0.4f, -10.0f, -3.0f);
    light3.intensity = 0.15f;
    auto light4 = SpotLight(light2); // Static Spot light
    light4.position = glm::vec3(-3.0f, 3.0f, 5.0f);
    light4.direction = -light4.position;
    light4.intensity = 0.0f;

    LightManager lights(defaultShader);
    lights.pointBucket.push_back(light0);
    lights.pointBucket.push_back(light1);
    lights.spotBucket.push_back(light2); // Flashlight
    lights.dirBucket.push_back(light3); // Moonlight
    lights.spotBucket.push_back(light4); // Static spot light
    lights.setAllLightSpaceMatrics();

    // set shadow id for point lights
    for (int i = 0; i < lights.pointBucket.size(); i++)
        lights.pointBucket[i].shadowId = i;

    // ------------------------- Initialize objects -------------------------

    Object object0(defaultShader, *meshMap["cube.obj"].get()); // Cube
    Object object1(object0); // Cube
    Object object2(object0); // Cube
    Object object3(object0); // Cube
    auto   object7 = Object(object0); // Cube
    Object object4(emissiveShader, *meshMap["sphere.obj"].get()); // Sphere
    Object object5(object4);                                               // Sphere
    Object object6(defaultShader, *meshMap["Floor.obj"].get());
    object0.rotation = glm::vec3(0.0f, -44.0f, 0.0f);
    object1.position += glm::vec3(1.8f, 0.3f, -1.3f);object1.rotation.z += 10;
    object2.position = glm::vec3(5.0f, 0.7f, 0.3f);object2.rotation.x += 8;object2.rotation.z += 15;
    object3.position = glm::vec3(3.0f, 0.1f, -1.0f);
    object4.position = light0.position;;
    object4.scale = glm::vec3(0.4);
    object5.position = light1.position;
    object5.scale = glm::vec3(0.4);
    object6.position = glm::vec3(0.0f, -1.0f, 0.0f);
    object6.scale    = glm::vec3(0.1);
    object7.position = glm::vec3(7.5f, 0.5f, 0.7f);
    auto object8 = object4; // Static Dir light physical
    object8.position = light4.position;
    object8.position.y += 1.0f;

    std::vector<Object> objects;
    objects.push_back(object0);
    objects.push_back(object1);
    objects.push_back(object2);
    objects.push_back(object3);
    objects.push_back(object4);
    objects.push_back(object5);
    objects.push_back(object6);
    objects.push_back(object7);
    objects.push_back(object8);

    // --- Shadow Frame Buffers ---
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    int width = SHADOW_WIDTH, height = SHADOW_HEIGHT;
    int shadowCount = 8; // Max number of 2D shadow maps
    int maxCubeMaps = 10; // x * 6 = total 2D textures

    GLuint shadow2dFBO;
    glGenFramebuffers(1, &shadow2dFBO);

    GLuint shadowCubeFBO;
    glGenFramebuffers(1, &shadowCubeFBO);

    // --- Create texture 2D Array ---
    GLuint textureArray;
    glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &textureArray);
    glTextureStorage3D(textureArray, 1, GL_DEPTH_COMPONENT24, width, height, shadowCount);
    // Standard shadow map filtering
    glTextureParameteri(textureArray, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(textureArray, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(textureArray, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTextureParameteri(textureArray, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    // Set border color for shadow maps to prevent artifacts outside frustum
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTextureParameterfv(textureArray, GL_TEXTURE_BORDER_COLOR, borderColor);

    // --- Create cube map array ---
    // TODO: create CubeMap class, let it handle arrays too
    GLuint cubeMapArray;
    glCreateTextures(GL_TEXTURE_CUBE_MAP_ARRAY, 1, &cubeMapArray);
    glTextureStorage3D(cubeMapArray, 1, GL_DEPTH_COMPONENT24, SHADOW_WIDTH, SHADOW_HEIGHT, maxCubeMaps * 6);
    // Set parameters
    glTextureParameteri(cubeMapArray, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(cubeMapArray, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(cubeMapArray, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(cubeMapArray, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(cubeMapArray, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glTextureParameteri(cubeMapArray, GL_TEXTURE_COMPARE_MODE, GL_NONE);

    // --- Attach Arrays to FBOs ---
    glBindFramebuffer(GL_FRAMEBUFFER, shadow2dFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, textureArray, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, shadowCubeFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, cubeMapArray, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //===================================================================================================
    // Render Loop
    // --------------------------------------------------------------------------------------------------
    while (!window.shouldClose()) {

        window.processInput(); // Window Inputs
        window.tick();         // Window Timing

        // Camera inputs
        camera.handleInputs(window);

        // Prepare shadow mapping passes
        // shadow2dShader.Activate();                         // Activate shadow shader
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT); // Set viewport size to shadow-map resolution

        // Render Shadow 2D Textures
        RenderContext::setPass(RenderPass::Shadow2D);        // Set renderer state to shadow pass
        glBindFramebuffer(GL_FRAMEBUFFER, shadow2dFBO); // Set frame-buffer to texture
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, textureArray, 0);
        renderer.renderScene(objects, lights, camera);// Render scene (shadow pass)


        // Render Shadow CubeMaps (Reduce performance by ~1/6)
        RenderContext::setPass(RenderPass::ShadowCube); // Set renderer state
        glBindFramebuffer(GL_FRAMEBUFFER, shadowCubeFBO); // Set frame-buffer to texture
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, cubeMapArray, 0);
        glClear(GL_DEPTH_BUFFER_BIT);
        renderer.renderScene(objects, lights, camera);// Render scene (shadow pass)

        // Reset renderer settings
        RenderContext::setPass(RenderPass::Main);       // Reset renderer state
        glBindFramebuffer(GL_FRAMEBUFFER, 0);  // Reset frame-buffer to default screen-buffer
        glViewport(0, 0, 800, 600);            // reset viewport size to default dimensions

        // Update Flashlight
        lights.spotBucket[0].direction = glm::normalize(camera.Orientation);
        lights.spotBucket[0].position  = camera.Position;

        // 2. Default Render
        defaultShader.Activate();
        defaultShader.setUniform("toggleF", window.f_toggle);
        lights.setAllLightSpaceMatrics();
        glBindTextureUnit(4, textureArray);
        glBindTextureUnit(5, cubeMapArray);


        // Main render pass
        renderer.renderScene(objects, lights, camera);

        // 3. Skybox Render
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



        window.swapBuffers();
    }
    // --------------------------------------------------------------------------------------------------

    return 0;
}
// =======================================================================================================
