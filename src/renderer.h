#ifndef LEARN_OPENGL_RENDERER_H
#define LEARN_OPENGL_RENDERER_H

#include <glm/glm.hpp>

#include "object.h"
#include "Mesh.h"
#include "camera.h"
#include "material.h"
#include "light.h"
#include "ubo.h"
#include "renderContext.h"
#include "fbo.h"
#include "betterTexture.h"

// STD
#include <vector>
#include <format>

/*
* 1. What is the Renderer's job?
The Renderer exists so your Object class doesn't need to know about cameras, clear colors,
or OpenGL state. It handles:

    Preparation      : Clearing the screen and setting the background color.
    State Management : Enabling/Disabling Depth Testing, Face Culling, or Blending.
    The Draw Call    : Taking an object, fetching its data, and pushing it to the GPU.
    Uniform Injection: Passing Global data (like the Camera's View/Projection matrices) into the shader.
*/



//----------------------------------------------------------------------------------------------------------------------
class Renderer {
//----------------------------------------------------------------------------------------------------------------------
private:
    // CONSTANTS
    static constexpr unsigned int SHADOW_MAP_RESOLUTION   = 1024;
    static constexpr          int MAX_2D_SHADOWS          = 8;   // Max number of 2D shadow maps
    static constexpr          int MAX_POINT_LIGHT_SHADOWS = 10;  // x * 6 = total 2D textures

    // ATTRIBUTES
    glm::vec4 clearColor{};
    bool      wireFrameMode;
    Shader*   activeShader;
    UBO*      uboLights;

    std::unique_ptr<ShadowFBO>     shadow2dFBO;   // Create texture framebuffer
    std::unique_ptr<ShadowFBO>     shadowCubeFBO; // Create cubemap framebuffer
    std::unique_ptr<BetterTexture> textureArray;
    std::unique_ptr<BetterTexture> cubemapArray;
    //----------------------------------------------------------------------------------------------------------------------

public:
    std::map<std::string, Shader*> shaderMap;
    Renderer(std::map<std::string, Shader*> shaderMap);  // Constructor
    ~Renderer(); // Destructor

    // METHODS
    int  initOpenGL();
    void prepare();
    void shadow2dPass(std::vector<Object>& objects, LightManager& lights, Camera& camera);
    void shadowCubePass(std::vector<Object>& objects, LightManager& lights, Camera& camera);
    void mainPass(std::vector<Object>& objects, LightManager& lights, Camera& camera);
    void shadow2dDraw(Object& object, LightManager& lights);
    void shadowCubeDraw(Object& object, LightManager& lights);
    void mainDraw(Object& obj, Camera& camera);
    void renderScene(std::vector<Object>& objects, LightManager& lights, Camera& camera);
    //----------------------------------------------------------------------------------------------------------------------

    void setWireframe(bool state);
    void setViewportSize(int width, int height);

    int getShadowMapResolution();
};
//----------------------------------------------------------------------------------------------------------------------

#endif //LEARN_OPENGL_RENDERER_H