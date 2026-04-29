#ifndef LEARN_OPENGL_RENDERER_H
#define LEARN_OPENGL_RENDERER_H

#include <glm/glm.hpp>

#include "object.h"
#include "Mesh.h"
#include "camera.h"
#include "material.h"
#include "light.h"
#include "ubo.h"

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

class Renderer {
private:
    // ATTRIBUTES
    glm::vec4 clearColor{};
    bool      wireFrameMode;
    Shader*   activeShader;
    UBO       uboLights;

public:
    Renderer();  // Constructor
    // ~Renderer(); // Destructor

    // METHODS
    int  initOpenGL();
    void prepare();
    void draw(Object& obj, Camera& camera);
    void renderScene(std::vector<Object>& objects,
                            LightManager& lights,
                                  Camera& camera,
                                  Shader& shader);

    void setWireframe(bool state);
    void setViewportSize(int width, int height);
};


#endif //LEARN_OPENGL_RENDERER_H