#include "renderer.h"

//-------------------------------------------------------------------------------------
// CONSTRUCTOR / DESTRUCTOR

Renderer::Renderer() {

    // Initialize attributes
    clearColor    = glm::vec4(0.07f, 0.13f, 0.17f, 1.0f);
    wireFrameMode = false;
    activeShader  = nullptr;

    // Initialize OpenGL
    initOpenGL();

    // Create Uniform Buffer
    uboLights = new UBO(sizeof(LightingData));
    uboLights->BindToSLot(0);
}

Renderer::~Renderer(){
    delete uboLights;
}

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

    // Enable SRGB Frame Buffer
    // glEnable(GL_FRAMEBUFFER_SRGB);

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

    // Apply default material
    // obj.mesh->materialList[0]->apply();

    // Activate material shader
    // activeShader = ;

    // Calculate model matrix
    auto modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, obj.position);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(obj.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(obj.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(obj.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    modelMatrix = glm::scale(modelMatrix, obj.scale);

    // // Set model matrix uniform
    // activeShader->setUniform("modelMatrix", modelMatrix);
    //
    // // set camera matrix uniform
    // camera.Matrix(*activeShader, "camMatrix");
    // activeShader->setUniform("viewPos", camera.Position);

    // Draw mesh
    obj.mesh->draw(camera, modelMatrix);

    // activeShader = nullptr;
}

// Batch rendering
void Renderer::renderScene(std::vector<Object>& objects,
                                  LightManager& lights,
                                        Camera& camera)
{
    // 1. Copy lighting data into contiguous memory
    LightingData lightData{};              // Will crash if real lights exceed struct's max lights
    for (int i = 0; i < lights.dirBucket.size(); i++) lightData.dirLights[i] = lights.dirBucket[i];
    for (int i = 0; i < lights.pointBucket.size(); i++) lightData.pointLights[i] = lights.pointBucket[i];
    for (int i = 0; i < lights.spotBucket.size(); i++) lightData.spotLights[i] = lights.spotBucket[i];
    lightData.dirCount   = lights.dirBucket.size();
    lightData.pointCount = lights.pointBucket.size();
    lightData.spotCount  = lights.spotBucket.size();
    uboLights->Bind();

    // 2.Set Uniform buffer (lighting data)
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(lightData), &lightData);

    // 3. Prepare the frame (Clear buffers)
    prepare();

    // 4. Object loop
    for (auto object: objects) {
            draw(object, camera);
    }
}

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