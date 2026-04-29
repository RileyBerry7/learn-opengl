#ifndef LEARN_OPENGL_LIGHT_H
#define LEARN_OPENGL_LIGHT_H


#include <glm/glm.hpp>
#include "shaderClass.h"
#include "texture.h"

//-------------------------------------------------------------------------------------

struct DirLight {         // Total: 32 bytes
    glm::vec3  direction; // 12 bytes
    float intensity;      // 4 bytes
    glm::vec3  color;     // 12 bytes
    float padding;        // 4  bytes
};
struct PointLight {      // Total: 48 bytes
    glm::vec3  position; // 12 bytes
    float intensity;     // 4  bytes
    glm::vec3  color;    // 12 bytes
    float constant;      // 4  bytes
    float linear;        // 4  bytes
    float quadratic;     // 4  bytes
    float radius;        // 4  bytes
    float padding;       // 4  bytes
};
struct SpotLight {       // Total: 48 bytes
    glm::vec3  position; // 12 bytes
    float intensity;     // 4  bytes
    glm::vec3  direction;// 12 bytes
    float cutOff;        // 4  bytes
    glm::vec3  color;    // 12 bytes
    float outerCutOff;   // 4  bytes
};
//----------------------------------------------------------------------------------------------------------------------

class LightManager {
public:
    std::vector<DirLight> dirBucket;
    std::vector<PointLight>       pointBucket;
    std::vector<SpotLight>        spotBucket;
    Shader* shader;

    LightManager(Shader& lightingShader) : shader(&lightingShader) {}
};
//----------------------------------------------------------------------------------------------------------------------
#define MAX_LIGHTS 10
struct LightingData {
    DirLight   dirLights[MAX_LIGHTS];   // 32 x n bytes
    PointLight pointLights[MAX_LIGHTS]; // 48 x n bytes
    SpotLight  spotLights[MAX_LIGHTS];  // 48 x n bytes
    alignas(16) int dirCount; // 4 bytes
    int pointCount;           // 4 bytes
    int spotCount;            // 4 bytes
};
//----------------------------------------------------------------------------------------------------------------------

#endif //LEARN_OPENGL_LIGHT_H