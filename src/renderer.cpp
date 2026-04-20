#include "renderer.h"

//-------------------------------------------------------------------------------------
// CONSTRUCTOR / DESTRUCTOR

Renderer::Renderer(){

    // Initialize attributes
    clearColor    = glm::vec4(0.07f, 0.13f, 0.17f, 1.0f);
    wireFrameMode = false;
    activeShader  = nullptr;

    // Initialize OpenGL
    initOpenGL();
}

// ~Renderer::Renderer(){
//
// }

//-------------------------------------------------------------------------------------
// METHODS

// Initialize GL settings (Depth test, Face culling, Blending)
int Renderer::initOpenGL(){

    // Default viewport size
    constexpr int defaultWidth  = 800;
    constexpr int defaultHeight = 600;

    // Load GLAD -> configures OpenGl to be driver agnostic
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }

    // Set default viewport size
    setViewportSize(defaultWidth, defaultHeight);

    // Set default wireframe rendering
    setWireframe(wireFrameMode);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);

    return 0;
}

// Refresh color/depth buffers
void Renderer::prepare(){

    // Replace background color
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// Render function
void Renderer::draw(Object& obj, Camera& camera){

    // Extract: Get the Material and Mesh from the Object.
    Material* material = obj.material;
    Mesh*     mesh     = obj.mesh;

    // Bind: Tell the Material to apply its shader and textures.
    material->apply();
    material->getShader(activeShader);

    // Upload: Calculate the Model matrix from the Object's position/rotation/scale and send it to the shader as a uniform.
    auto modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, obj.position);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(obj.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(obj.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(obj.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    modelMatrix = glm::scale(modelMatrix, obj.scale);
    activeShader->setUniform("modelMatrix", modelMatrix);

    // Set Camera Matrix Uniform
    camera.Matrix(*activeShader, "camMatrix");
    activeShader->setUniform("viewPos", camera.Position);

    // Set Required Uniforms


    // Execute: Tell the Mesh to call its draw command
    mesh->draw();

    activeShader = nullptr;
}

// Batch rendering
// void renderScene(std::vector<Object>& objects, std::vector<Light>& lights, Camera& camera){
// }

//--------------------------------------------------------------------------------------------
// CONTROL

void Renderer::setWireframe(bool state){
    wireFrameMode = state;
    if (wireFrameMode) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void Renderer::setViewportSize(int width, int height){
    glViewport(0, 0, width, height);
}