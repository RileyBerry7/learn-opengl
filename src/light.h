#ifndef LEARN_OPENGL_LIGHT_H
#define LEARN_OPENGL_LIGHT_H


#include <glm/glm.hpp>
#include "shaderClass.h"
#include "texture.h"

//-------------------------------------------------------------------------------------

struct alignas(16) DirLight {        // Total: 96 bytes
    glm::vec3 direction; // 12 bytes
    float     intensity; // 4 bytes
    glm::vec3 color;     // 12 bytes
    int       shadowId;  // 4  bytes
    glm::mat4 lightSpaceMatrix; // 64 bytes
};
struct alignas(16) PointLight {      // Total: 112 bytes
    glm::vec3  position; // 12 bytes
    float intensity;     // 4  bytes
    glm::vec3  color;    // 12 bytes
    float constant;      // 4  bytes
    float linear;        // 4  bytes
    float quadratic;     // 4  bytes
    float radius;        // 4  bytes
    int   shadowId;      // 4  bytes
    glm::mat4  leftLSM;  // 64 bytes
    glm::mat4  rightLSM; // 64 bytes
    glm::mat4  frontLSM; // 64 bytes
    glm::mat4  backLSM;  // 64 bytes
    glm::mat4  topLSM;   // 64 bytes
    glm::mat4  bottomLSM;// 64 bytes
};
struct alignas(16) SpotLight {       // Total: 128 bytes
    glm::vec3  position; // 12 bytes
    float intensity;     // 4  bytes
    glm::vec3  direction;// 12 bytes
    float cutOff;        // 4  bytes
    glm::vec3  color;    // 12 bytes
    float outerCutOff;   // 4  bytes
    int   shadowId;      // 4 bytes
    float padding1;      // 4 bytes
    float padding2;      // 4 bytes
    float padding3;      // 4 bytes
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
        for (SpotLight& light : spotBucket) {
            light.lightSpaceMatrix = calcLightSpaceMatrix(light);
        }
        for (PointLight& light : pointBucket) {
            setPointLSM(light);
        }
    }
    glm::mat4 calcLightSpaceMatrix(DirLight &light) {
        float near_plane = 0.1f, far_plane = 50.0f;
        glm::mat4 lightProjection = glm::ortho(-25.0f, 25.0f, -25.0f, 25.0f, near_plane, far_plane);
        glm::vec3 lightPos = glm::vec3(0.0f) - glm::normalize(light.direction) * 25.0f;
        glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        return lightProjection * lightView;
    }
    void setPointLSM(PointLight &light)
    {
        float near_plane = 0.1f;
        float far_plane  = light.radius;
        float aspect     = 1.0f;

        glm::mat4 proj = glm::perspective(glm::radians(90.0f), aspect, near_plane, far_plane);
        glm::vec3 pos = light.position;

        // +X right
        light.rightLSM = proj * glm::lookAt(pos, pos + glm::vec3(1, 0, 0), glm::vec3(0,-1, 0));
        // -X left
        light.leftLSM = proj * glm::lookAt(pos, pos + glm::vec3(-1, 0, 0), glm::vec3(0,-1, 0));
        // +Y top
        light.topLSM = proj * glm::lookAt(pos, pos + glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));
        // -Y bottom
        light.bottomLSM = proj * glm::lookAt(pos, pos + glm::vec3(0,-1, 0), glm::vec3(0, 0,-1));
        // +Z front
        light.frontLSM = proj * glm::lookAt(pos, pos + glm::vec3(0, 0, 1), glm::vec3(0,-1, 0));
        // -Z back
        light.backLSM = proj * glm::lookAt(pos, pos + glm::vec3(0, 0,-1),glm::vec3(0,-1, 0));
    }
    glm::mat4 calcLightSpaceMatrix(SpotLight &light) {
    float near_plane = 0.1f;
    float far_plane = 50.0f;
    float aspect = 1.0f;
    // 1. Get the actual half-angle of the cone
    float halfConeAngle = std::acos(light.outerCutOff);
    // 2. Correctly scale the tangent to cover the diagonal corners of the square texture
    float fov = 2.0f * std::atan(std::tan(halfConeAngle) * 1.41421356f);
    // 3. Prevent mathematical explosion if the spotlight is close to 180 degrees
    fov = glm::clamp(fov, 0.1f, glm::radians(175.0f));
    glm::mat4 lightProjection = glm::perspective(fov, aspect, near_plane, far_plane);
    glm::vec3 safeDir = (glm::length(light.direction) > 0.001f) ? glm::normalize(light.direction) : glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 targetPos = light.position + safeDir;
    glm::vec3 up = (std::abs(safeDir.y) > 0.99f) ? glm::vec3(0.0f, 0.0f, -1.0f) : glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 lightView = glm::lookAt(light.position, targetPos, up);
    return lightProjection * lightView;
}

};
//----------------------------------------------------------------------------------------------------------------------
#define MAX_LIGHTS 10
struct alignas(16) LightingData {
    DirLight   dirLights[MAX_LIGHTS];   // 96 x n bytes
    PointLight pointLights[MAX_LIGHTS]; // 112 x n byte
    SpotLight  spotLights[MAX_LIGHTS];  // 128 x n bytes
    alignas(16) int dirCount; // 4 bytes
    int pointCount;           // 4 bytes
    int spotCount;            // 4 bytes
    int padding;
};
//----------------------------------------------------------------------------------------------------------------------

#endif //LEARN_OPENGL_LIGHT_H