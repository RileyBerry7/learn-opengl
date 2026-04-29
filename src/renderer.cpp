#include "renderer.h"

//-------------------------------------------------------------------------------------
// CONSTRUCTOR / DESTRUCTOR

Renderer::Renderer(){

    // Initialize attributes
    clearColor    = glm::vec4(0.07f, 0.13f, 0.17f, 1.0f);
    wireFrameMode = false;
    activeShader  = nullptr;
    uboLights     = UBO(1296, nullptr);
    uboLights.BindToSLot(0);

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

    // Execute: Tell the Mesh to call its draw command
    mesh->draw();

    activeShader = nullptr;
}

// Batch rendering
void Renderer::renderScene(std::vector<Object>& objects,
                                  LightManager& lights,
                                        Camera& camera,
                                        Shader& shader) {
    // 0. Uniform Buffers

    // Copy lighting data into contiguous memory
    LightingData lightData{};
    for (int i = 0; i < lights.dirBucket.size(); i++) lightData.dirLights[i] = lights.dirBucket[i];
    for (int i = 0; i < lights.pointBucket.size(); i++) lightData.pointLights[i] = lights.pointBucket[i];
    for (int i = 0; i < lights.spotBucket.size(); i++) lightData.spotLights[i] = lights.spotBucket[i];
    lightData.dirCount   = lights.dirBucket.size();
    lightData.pointCount = lights.pointBucket.size();
    lightData.spotCount  = lights.spotBucket.size();

    glBufferSubData(GL_UNIFORM_BUFFER, 0,
        lights.pointBucket.size() * sizeof(PointLight),
        lights.pointBucket.data());

    // 1. Prepare the frame (Clear buffers)
    prepare();

    // 2. Identify the Shader
    shader.Activate(); // This is temporary

    // 3. THE INJECTION POINT
    shader.setUniform("lightCount", static_cast<int>(lights.size()));
    for (int i = 0; i < lights.size(); i++) {

        // Universal light uniforms
        std::string indexStr = std::format("lights[{}].", i);
        shader.setUniform((indexStr + "type"    ).c_str(),  static_cast<GLuint>(lights[i]->type));
        shader.setUniform((indexStr + "ambient" ).c_str(),  lights[i]->ambient);
        shader.setUniform((indexStr + "diffuse" ).c_str(),  lights[i]->diffuse);
        shader.setUniform((indexStr + "specular").c_str(),  lights[i]->specular);

        // Directional light uniforms
        if (lights[i]->type == LightType::Directional) {
            auto dLight = static_cast<DirectionalLight&>(*lights[i]);
            shader.setUniform((indexStr + "direction").c_str(),  dLight.direction);

            // Point light uniforms
        } else if (lights[i]->type == LightType::Point) {
            auto pLight = static_cast<PointLight&>(*lights[i]);
            shader.setUniform((indexStr + "position").c_str(),  pLight.position);
            shader.setUniform((indexStr + "constant").c_str(),  pLight.constant);
            shader.setUniform((indexStr + "linear").c_str(),    pLight.linear);
            shader.setUniform((indexStr + "quadratic").c_str(), pLight.quadratic);

            // Spot light uniforms
        } else if (lights[i]->type == LightType::Spot) {
            auto sLight = static_cast<SpotLight&>(*lights[i]);
            shader.setUniform((indexStr + "position"   ).c_str(),  sLight.position);
            shader.setUniform((indexStr + "direction"  ).c_str(),  sLight.direction);
            shader.setUniform((indexStr + "cutOff"     ).c_str(),  sLight.cutOff);
            shader.setUniform((indexStr + "outerCutOff").c_str(),  sLight.outerCutOff);
        }
    }

    // 4. THE OBJECT LOOP
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