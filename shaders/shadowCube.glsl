//======================================================================================================================
#type vertex
#version 430 core
#extension GL_ARB_shader_viewport_layer_array : require
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
    int   shadowId;  // 4  bytes
    mat4  lightSpaceMatrix; // 64 bytes
};
struct PointLight {     //
    vec3  position;     // 12 bytes
    float intensity;    // 4  bytes
    vec3  color;        // 12 bytes
    float constant;     // 4  bytes
    float linear;       // 4  bytes
    float quadratic;    // 4  bytes
    float radius;       // 4  bytes
    int   shadowId;     // 4  bytes
    mat4  leftLSM;  // 64 bytes
    mat4  rightLSM; // 64 bytes
    mat4  frontLSM; // 64 bytes
    mat4  backLSM;  // 64 bytes
    mat4  topLSM;   // 64 bytes
    mat4  bottomLSM;// 64 bytes
};
struct SpotLight {    // Total: 128 bytes
    vec3  position;   // 12 bytes
    float intensity;  // 4  bytes
    vec3  direction;  // 12 bytes
    float cutOff;     // 4  bytes
    vec3  color;      // 12 bytes
    float outerCutOff;// 4  bytes
    int   shadowId;         // 4 bytes
    float padding1;         // 4 bytes
    float padding2;         // 4 bytes
    float padding3;         // 4 bytes
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


//======================================================================================================================
layout (location = 0) in vec3 aPos;
uniform mat4 modelMatrix;
uniform int  cubeMapIndex;
out vec3     fragPos;
flat out vec3  lightPos;  // The world-space position of the point light
flat out float farPlane;  // The far clipping plane of the light's projection matrix

void main() {

    int lightIndex   = cubeMapIndex;  // Light index
    int faceIndex    = gl_InstanceID; // Face  index
    gl_Layer         = (6 * lightIndex) + faceIndex;
    vec4 worldPos    = modelMatrix * vec4(aPos, 1.0); //
    PointLight light = pointLights[lightIndex];       //

    mat4 lightSpaceMatrix;
    switch(faceIndex){
        case 0: lightSpaceMatrix = light.rightLSM;
                break;
        case 1: lightSpaceMatrix = light.leftLSM;
                break;
        case 2: lightSpaceMatrix = light.topLSM;
                break;
        case 3: lightSpaceMatrix = light.bottomLSM;
                break;
        case 4: lightSpaceMatrix = light.frontLSM;
                break;
        case 5: lightSpaceMatrix = light.backLSM;
                break;
    }
    // Compute final position using the specific face matrix
    gl_Position = lightSpaceMatrix * worldPos;
    fragPos  = vec3(worldPos);
    lightPos = light.position;
    farPlane = light.radius;
}
//----------------------------------------------------------------------------------------------------------------------



//======================================================================================================================
#type fragment
#version 430 core

in vec3 fragPos; // We need the world-space position from the vertex shader
flat in vec3 lightPos;   // The world-space position of the point light
flat in float farPlane;  // The far clipping plane of the light's projection matrix

void main() {
    float lightDistance = length(fragPos - lightPos); // Distance (light -> fragment)

    // Map to [0, 1] range by dividing by farPlane
    lightDistance = lightDistance / farPlane;

    // Write this as the explicit depth value
    gl_FragDepth = lightDistance;
}
//----------------------------------------------------------------------------------------------------------------------
