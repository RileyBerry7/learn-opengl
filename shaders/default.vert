#version 420 core
//----------------------------------------------------------------------------------------------------------------------
// TYPE DEFINITIONS
struct Material {
    sampler2D diffuse;   // Diffuse map  (texture)
    sampler2D specular;  // Specular map (texture)
    float     shininess; // Surface gloss level
};
/* In GLSL data storage sizes are calculated based on 16-byte chunks. All elements reserve 16 bytes.
   Thus our struct size should be a factor of 16 in order to ensure alignment of contiguos
   memory, that is our uniform buffer arrays will perfectly map to the GLSL interface.
*/
struct DirLight {    // Total: 96 bytes
    vec3  direction; // 12 bytes
    float intensity; // 4 bytes
    vec3  color;     // 12 bytes
    float padding;   // 4  bytes
    mat4  lightSpaceMatrix; // 64 bytes
};
struct PointLight {  // Total: 112 bytes
    vec3  position;  // 12 bytes
    float intensity; // 4  bytes
    vec3  color;     // 12 bytes
    float constant;  // 4  bytes
    float linear;    // 4  bytes
    float quadratic; // 4  bytes
    float radius;    // 4  bytes
    float padding;   // 4  bytes
    mat4  lightSpaceMatrix; // 64 bytes
};
struct SpotLight {    // Total: 112 bytes
    vec3  position;   // 12 bytes
    float intensity;  // 4  bytes
    vec3  direction;  // 12 bytes
    float cutOff;     // 4  bytes
    vec3  color;      // 12 bytes
    float outerCutOff;// 4  bytes
    mat4  lightSpaceMatrix; // 64 bytes
};
//----------------------------------------------------------------------------------------------------------------------
// CONFIGURATION
#define MAX_LIGHTS 10
//----------------------------------------------------------------------------------------------------------------------
// UNIFORM BUFFERS
layout (std140, binding = 0 ) uniform LightData { // Lighting data block
    DirLight   dirLights[MAX_LIGHTS];   // Array of structs
    PointLight pointLights[MAX_LIGHTS]; // Array of structs
    SpotLight  spotLights[MAX_LIGHTS];  // Array of structs
    int dirCount, pointCount, spotCount;// Array element counts
};
//----------------------------------------------------------------------------------------------------------------------
// UNIFORMS
uniform mat4 camMatrix;
uniform mat4 modelMatrix;
//----------------------------------------------------------------------------------------------------------------------
// INPUT - vertex attributes from VBO / VAO
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTex;
layout (location = 3) in vec3 aNormal;
//----------------------------------------------------------------------------------------------------------------------
// OUTPUT
out vec3 objColor;
out vec2 texCoord;
out vec3 normal;
out vec3 fragPos;
//======================================================================================================================
void main()
{
    // Apply scale and camera matrix
    gl_Position = camMatrix * modelMatrix * vec4(aPos, 1.0);
    objColor    = aColor;
    texCoord    = aTex;

    // The Normal Matrix: inverse transpose of the 3x3 model matrix
    normal = mat3(transpose(inverse(modelMatrix))) * aNormal;
    fragPos = vec3(modelMatrix * vec4(aPos, 1.0));

//    fragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0); // Moved to frag shader


}
//======================================================================================================================
