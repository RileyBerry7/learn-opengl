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
struct PointLight {     // Total: 112 bytes
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
//----------------------------------------------------------------------------------------------------------------------
