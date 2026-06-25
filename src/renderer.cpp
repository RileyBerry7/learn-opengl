#include "renderer.h"

//-------------------------------------------------------------------------------------
// CONSTRUCTOR / DESTRUCTOR

//----------------------------------------------------------------------------------------------------------------------
Renderer::Renderer(std::map<std::string, Shader*> shaderMap) :
    shaderMap(std::move(shaderMap)) {

    // Initialize attributes
    clearColor    = glm::vec4(0.07f, 0.13f, 0.17f, 1.0f);
    wireFrameMode = false;
    activeShader  = nullptr;

    // Initialize OpenGL
    initOpenGL();

    // Create uniform buffers
    uboLights = new UBO(sizeof(LightingData)); // Light UBO
    uboLights->BindToSLot(0);

    // ========== Shadow Mapping Setup ==========
    shadow2dFBO   = std::make_unique<ShadowFBO>(SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION);
    shadowCubeFBO = std::make_unique<ShadowFBO>(SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION);

    // --- Configure 2D Texture Array ---
    textureArray = std::make_unique<BetterTexture>(GL_TEXTURE_2D_ARRAY);
    textureArray->create2DArray(SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION, MAX_2D_SHADOWS, GL_DEPTH_COMPONENT24);
    textureArray->setFilter(GL_NEAREST, GL_NEAREST);
    textureArray->setWrap(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
    textureArray->setBorderColor(1.0f, 1.0f, 1.0f, 1.0f);

    // --- Configure Cube Map Array ---
    cubemapArray = std::make_unique<BetterTexture>(GL_TEXTURE_CUBE_MAP_ARRAY);
    cubemapArray->createCubemapArray(SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION, MAX_POINT_LIGHT_SHADOWS, GL_DEPTH_COMPONENT24);
    cubemapArray->setFilter(GL_LINEAR, GL_LINEAR);
    cubemapArray->setWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    cubemapArray->setBorderColor(1.0f, 1.0f, 1.0f, 1.0f);
    glTextureParameteri(cubemapArray->getID(), GL_TEXTURE_COMPARE_MODE, GL_NONE);

    shadow2dFBO->attachDepthArray(textureArray->getID());     // Attach texture array to FBO
    shadowCubeFBO->attachCubemapArray(cubemapArray->getID()); // Attach cubemap array to FBO

    // Initialize skybox mesh
    skyboxMesh = std::make_unique<Mesh>("cube.obj", shaderMap);
}

//----------------------------------------------------------------------------------------------------------------------
Renderer::~Renderer(){
    delete uboLights;
}

//-------------------------------------------------------------------------------------
// INITIALIZE OPENGL - Initialize GL settings (Depth test, Face culling, Blending)
int Renderer::initOpenGL(){

    // Default viewport size
    constexpr int defaultWidth  = 800;
    constexpr int defaultHeight = 600;

    // Load GLAD -> configures OpenGl to be driver agnostic
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }
    setViewportSize(defaultWidth, defaultHeight); // Set default viewport size
    setWireframe(wireFrameMode);                  // Set default wireframe rendering
    glEnable(GL_DEPTH_TEST);                  // Enable depth test

    return 0; // Successful return
}

//----------------------------------------------------------------------------------------------------------------------
// PREPARE - Refresh color/depth buffers
void Renderer::prepare(){

    // Replace background color
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


//----------------------------------------------------------------------------------------------------------------------
// SHADOW CUBE PASS
void Renderer::shadowCubePass(std::vector<Object>& objects, LightManager& lights, Camera& camera){

    RenderContext::setPass(RenderPass::ShadowCube); // Set renderer state
    shadowCubeFBO->bind();
    glClear(GL_DEPTH_BUFFER_BIT);
    renderScene(objects, lights, camera);// Render scene (shadow pass)
}

//----------------------------------------------------------------------------------------------------------------------
// SHADOW 2D PASS
void Renderer::shadow2dPass(std::vector<Object>& objects, LightManager& lights, Camera& camera) {

    RenderContext::setPass(RenderPass::Shadow2D);        // Set renderer state to shadow pass
    shadow2dFBO->bind();
    // glBindFramebuffer(GL_FRAMEBUFFER, shadow2dFBO); // Set frame-buffer to texture
    renderScene(objects, lights, camera);// Render scene (shadow pass)
}
//--------------------------------------------------------------------------------------------
// MAIN PASS
void Renderer::mainPass(std::vector<Object>& objects, LightManager& lights, Camera& camera) {

    activeShader = shaderMap["default"];
    textureArray->bindUnit(4);
    cubemapArray->bindUnit(5);
    RenderContext::setPass(RenderPass::Main);             // Set renderer state
    glBindFramebuffer(GL_FRAMEBUFFER, 0);  // Set frame-buffer to default screen-buffer
    lights.setAllLightSpaceMatrics();
    renderScene(objects, lights, camera);
}

//----------------------------------------------------------------------------------------------------------------------
// SHADOW 2D DRAW
void Renderer::shadow2dDraw(Object& object, LightManager& lights) {
    const auto modelMatrix = object.getModelMatrix();
    object.mesh->vao->Bind();
    Shader* shadow2dShader = shaderMap[std::string("shadow2d")];
    shadow2dShader->Activate();
    shadow2dShader->setUniform("modelMatrix", modelMatrix);

    int shadowIndex = 0;
    for (DirLight& light : lights.dirBucket) {
        light.shadowId = shadowIndex;
        std::string uniformName = std::format("lightSpaceMatrices[{}]", shadowIndex++);
        shadow2dShader->setUniform(uniformName.c_str(), light.lightSpaceMatrix);
    }
    for (SpotLight& light : lights.spotBucket) {
        light.shadowId = shadowIndex;
        std::string uniformName = std::format("lightSpaceMatrices[{}]", shadowIndex++);
        shadow2dShader->setUniform(uniformName.c_str(), light.lightSpaceMatrix);
    }

    glDrawElementsInstanced(GL_TRIANGLES, object.mesh->index_count,
                             GL_UNSIGNED_INT, (void*)0,shadowIndex);
    object.mesh->vao->Unbind();
    }
    //----------------------------------------------------------------------------------------------------------------------
    // SHADOW CUBE DRAW
    void Renderer::shadowCubeDraw(Object& object, LightManager& lights)
{
    object.mesh->vao->Bind();

    Shader* shader = shaderMap["shadowCube"];
    shader->Activate();

    shader->setUniform("modelMatrix", object.getModelMatrix());

    // Point light loop
    for (int i = 0; i < lights.pointBucket.size(); i++) {
        shader->setUniform("cubeMapIndex", i);
        int faceCount = 6;
        glDrawElementsInstanced(GL_TRIANGLES, object.mesh->index_count, GL_UNSIGNED_INT, nullptr, faceCount);
    }
    object.mesh->vao->Unbind();
}
//--------------------------------------------------------------------------------------------
// MAIN DRAW - draw each mesh
void Renderer::mainDraw(Object& obj, Camera& camera)
{
    const auto modelMatrix = obj.getModelMatrix();
    obj.mesh->drawBatches(camera, modelMatrix); // Renders each material-submesh
}
//----------------------------------------------------------------------------------------------------------------------
// RENDER SCENE - draw each object
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
    for (auto& object: objects) {
        switch (RenderContext::getPass()) {
            case RenderPass::Main:
            mainDraw(object, camera);
                break;
            case RenderPass::Shadow2D:
                shadow2dDraw(object, lights);
                break;
            case RenderPass::ShadowCube:
                shadowCubeDraw(object, lights);
                break;
        }
    }
}

//--------------------------------------------------------------------------------------------
// DRAW SKYBOX
void Renderer::drawSkybox(BetterTexture& skybox, Camera& camera) {

    glDepthFunc(GL_LEQUAL); // Allow drawing at depth 1.0
    Shader* skyboxShader = shaderMap["skybox"];
    skyboxShader->Activate();
    skyboxMesh->vao->Bind();
    glm::mat4 view = glm::mat4(glm::mat3(camera.view)); // Strip movement!
    skybox.bindUnit(0);
    skyboxShader->setUniform("projection", camera.projection);
    skyboxShader->setUniform("view", view);
    glDrawElements(GL_TRIANGLES, skyboxMesh->index_count,GL_UNSIGNED_INT, 0);
    glDepthFunc(GL_LESS); // Reset depth
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

//----------------------------------------------------------------------------------------------------------------------
void Renderer::setViewportSize(int width, int height){
    glViewport(0, 0, width, height);
}
//----------------------------------------------------------------------------------------------------------------------
int Renderer::getShadowMapResolution() {
    return SHADOW_MAP_RESOLUTION;
}