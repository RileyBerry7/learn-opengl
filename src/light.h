#ifndef LEARN_OPENGL_LIGHT_H
#define LEARN_OPENGL_LIGHT_H


#include <glm/glm.hpp>
#include "shaderClass.h"
#include "texture.h"

//-------------------------------------------------------------------------------------

struct DirLight {         // Total: 96 bytes
    glm::vec3  direction; // 12 bytes
    float intensity;      // 4 bytes
    glm::vec3  color;     // 12 bytes
    float padding;        // 4  bytes
    glm::mat4  lightSpaceMatrix; // 64 bytes
};
struct PointLight {      // Total: 112 bytes
    glm::vec3  position; // 12 bytes
    float intensity;     // 4  bytes
    glm::vec3  color;    // 12 bytes
    float constant;      // 4  bytes
    float linear;        // 4  bytes
    float quadratic;     // 4  bytes
    float radius;        // 4  bytes
    float padding;       // 4  bytes
    glm::mat4  lightSpaceMatrix; // 64 bytes
};
struct SpotLight {       // Total: 112 bytes
    glm::vec3  position; // 12 bytes
    float intensity;     // 4  bytes
    glm::vec3  direction;// 12 bytes
    float cutOff;        // 4  bytes
    glm::vec3  color;    // 12 bytes
    float outerCutOff;   // 4  bytes
    glm::mat4  lightSpaceMatrix; // 64 bytes
};
//----------------------------------------------------------------------------------------------------------------------

class LightManager {
public:
    std::vector<DirLight>   dirBucket;
    std::vector<PointLight> pointBucket;
    std::vector<SpotLight>  spotBucket;
    Shader* shader;

    LightManager(Shader& lightingShader) : shader(&lightingShader) {}

    void setAllLightSpaceMatrics() {
        for (DirLight& light : dirBucket) {
            light.lightSpaceMatrix = calcLightSpaceMatrix(light);
        }
    }
    glm::mat4 calcLightSpaceMatrix(DirLight &light) {
        float near_plane = 0.1f, far_plane = 50.0f;
        glm::mat4 lightProjection = glm::ortho(-25.0f, 25.0f, -25.0f, 25.0f, near_plane, far_plane);
        glm::vec3 lightPos = glm::vec3(0.0f) - glm::normalize(light.direction) * 25.0f;
        glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        return lightProjection * lightView;
    }
};
//----------------------------------------------------------------------------------------------------------------------
#define MAX_LIGHTS 10
struct LightingData {
    DirLight   dirLights[MAX_LIGHTS];   // 96 x n bytes
    PointLight pointLights[MAX_LIGHTS]; // 112 x n bytes
    SpotLight  spotLights[MAX_LIGHTS];  // 112 x n bytes
    alignas(16) int dirCount; // 4 bytes
    int pointCount;           // 4 bytes
    int spotCount;            // 4 bytes
};
//----------------------------------------------------------------------------------------------------------------------

#endif //LEARN_OPENGL_LIGHT_H