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
struct DirLight {    // Total: 32 bytes
    vec3  direction; // 12 bytes
    float intensity; // 4 bytes
    vec3  color;     // 12 bytes
    float padding;   // 4  bytes
};
struct PointLight {   // Total: 48 bytes
    vec3  position;  // 12 bytes
    float intensity; // 4  bytes
    vec3  color;     // 12 bytes
    float constant;  // 4  bytes
    float linear;    // 4  bytes
    float quadratic; // 4  bytes
    float radius;    // 4  bytes
    float padding;   // 4  bytes
};
struct SpotLight {    // Total: 48 bytes
    vec3  position;   // 12 bytes
    float intensity;  // 4  bytes
    vec3  direction;  // 12 bytes
    float cutOff;     // 4  bytes
    vec3  color;      // 12 bytes
    float outerCutOff;// 4  bytes
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
uniform sampler2D tex0;     // Texture uniform
uniform vec3      viewPos;  // View position uniform
uniform Material  material; // Material uniform
//----------------------------------------------------------------------------------------------------------------------
// INPUT
in vec3 objColor;
in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;
//----------------------------------------------------------------------------------------------------------------------
// OUTPUT
out vec4 FragColor;
//----------------------------------------------------------------------------------------------------------------------
// FUNCTION PROTOTYPES
vec3 calculateDirLight(DirLight   light, vec3 normal, vec3 viewDir, vec3 specMap);
vec3 calculatePointLight(PointLight   light, vec3 normal, vec3 viewDir, vec3 specMap, vec3 fragPos);
vec3 calculateSpotLight(SpotLight   light, vec3 normal, vec3 viewDir, vec3 specMap, vec3 fragPos);
vec3 calcSpecular(vec3 normal, vec3 lightDir, vec3 viewDir, float materialShine, vec3 lightEnergy, vec3 specMap);
//======================================================================================================================
void main()
{
    vec3 totalLight = vec3(0.0); // Light color accumulator

    // Calculate loop invariants
    vec3 norm     = normalize(normal);
    vec3 viewDir  = normalize(viewPos -fragPos);
    vec3 albedo   = pow(texture(material.diffuse, texCoord).rgb, vec3(2.2));//remove gamma from texture
    vec3 specMap  = texture(material.specular, texCoord).rgb;
    vec3 ambient  = albedo * 0.05;

    // Calculate total light
    for (int i = 0; i < dirCount; i++)
        totalLight += calculateDirLight(dirLights[i], norm, viewDir, specMap);
    for (int i = 0; i < pointCount; i++)
        totalLight += calculatePointLight(pointLights[i], norm, viewDir, specMap, fragPos);
    for (int i = 0; i < spotCount; i++)
        totalLight += calculateSpotLight(spotLights[i], norm, viewDir, specMap, fragPos);

    vec3 finalColor = (totalLight + ambient) * albedo * objColor;// Combine color components
    finalColor = finalColor / (finalColor + vec3(1.0));// Reinhard tone mapping
    finalColor = pow(finalColor, vec3(1.0/2.2));       // Gamma correction
    FragColor = vec4(finalColor, 1.0);                 // Append alpha channel
}
//===================================V===================================================================================

//----------------------------------
//  DIRECTIONAL LIGHT CALCULATION  -
//----------------------------------
vec3 calculateDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 specMap) {

    vec3 lightEnergy = light.color * light.intensity;// Total emission power
    vec3 lightDir = normalize(-light.direction);     // Direction->Light vector
    float diff    = max(dot(normal, lightDir), 0.0); // Diffuse lighting
    vec3 diffuse  = lightEnergy * diff;              // Diffuse lighting
    vec3 specular = calcSpecular(normal, lightDir, viewDir, material.shininess, lightEnergy, specMap);// Specular lighting

    return diffuse + specular;
}

//----------------------------
//  POINT LIGHT CALCULATION  -
//----------------------------
vec3 calculatePointLight(PointLight light, vec3 normal, vec3 viewDir, vec3 specMap, vec3 fragPos) {

    vec3 lightEnergy = light.color * light.intensity;   // Total emissive power
    vec3 lightDir = normalize(light.position - fragPos);// Frag->Light vector
    float diff    = max(dot(normal, lightDir), 0.0);    // Diffuse lighting
    vec3 diffuse  = lightEnergy * diff;                 // Diffuse lighting
    vec3 specular = calcSpecular(normal, lightDir, viewDir, material.shininess, lightEnergy, specMap);// Specular lighting

    // Attenuation: intensity loss over distance
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    return (diffuse + specular) * attenuation;
}

//---------------------------
//  SPOT LIGHT CALCULATION  -
//---------------------------
vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 viewDir, vec3 specMap, vec3 fragPos) {

    vec3 lightEnergy = light.color * light.intensity;   // Total emissive power
    vec3 lightDir = normalize(light.position - fragPos);// Frag->Light vector
    float diff    = max(dot(normal, lightDir), 0.0);    // Diffuse lighting
    vec3 diffuse  = lightEnergy * diff;                 // Diffuse lighting
    vec3 specular = calcSpecular(normal, lightDir, viewDir, material.shininess, lightEnergy, specMap);// Specular lighting

    // Soft spotlight intensity calculation
    vec3 spotDir  = normalize(light.direction);// Spotlight direction vector
    float theta   = dot(lightDir, -spotDir);   // scalar product
    float phi     = light.cutOff;              // cos(inner_angle)
    float epsilon = phi - light.outerCutOff;   // cosine difference: cos(in) - cos(out)
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);// Formula

    return (diffuse + specular) * intensity;
}

//------------------------
// SPECULAR CALCULATION  -
//------------------------
vec3 calcSpecular(vec3 normal, vec3 lightDir, vec3 viewDir, float materialShine, vec3 lightEnergy, vec3 specMap) {
    float  energyConservation = (materialShine   + 2.0) / 8.0;// Scalar that increases as specular area decreases
    vec3   reflectDir = reflect(-lightDir, normal); // Frag->Reflection vector
    float  spec       = pow(max(dot(viewDir, reflectDir), 0.0), materialShine);
    return lightEnergy * spec * specMap * energyConservation;
}
//======================================================================================================================
