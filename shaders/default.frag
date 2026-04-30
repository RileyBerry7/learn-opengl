#version 420 core
//----------------------------------------------------------------------------------------------------------------------
// Type Definitions

struct Material {
    sampler2D diffuse;   // Diffuse map (texture)
    sampler2D specular;  // Specuar map (texture)
    float     shininess; //
};
// In GLSL sizes are calculated based on 16-byte chunks. All elements reserve 16 bytes. Thus our struct size should be
// a factor of 16 in order to ensure alignment of contiguos memory(of our light arrays).
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
vec3 calculateDirLight(DirLight   light, vec3 normal, vec3 viewDir, vec3 albedo, vec3 specMap, vec3 fragPos);
vec3 calculatePointLight(PointLight   light, vec3 normal, vec3 viewDir, vec3 albedo, vec3 specMap, vec3 fragPos);
vec3 calculateSpotLight(SpotLight   light, vec3 normal, vec3 viewDir, vec3 albedo, vec3 specMap, vec3 fragPos);

//======================================================================================================================
void main()
{
    vec3 totalLight = vec3(0.0); // Light color accumulator

    // Calculate loop invariants
    vec3 norm     = normalize(normal);
    vec3 viewDir  = normalize(viewPos -fragPos);
    vec3 albedo   = texture(material.diffuse, texCoord).rgb;
    vec3 specMap  = texture(material.specular, texCoord).rgb;

    // Calculate total light
    for (int i = 0; i < dirCount; i++)
        totalLight += calculateDirLight(dirLights[i], norm, viewDir, albedo, specMap, fragPos);
    for (int i = 0; i < pointCount; i++)
        totalLight += calculatePointLight(pointLights[i], norm, viewDir, albedo, specMap, fragPos);
    for (int i = 0; i < spotCount; i++)
        totalLight += calculateSpotLight(spotLights[i], norm, viewDir, albedo, specMap, fragPos);

    FragColor = vec4((totalLight * objColor), 1.0); // Output
}
//===================================V===================================================================================

//----------------------------------
//  DIRECTIONAL LIGHT CALCULATION  -
//----------------------------------
vec3 calculateDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 albedo, vec3 specMap, vec3 fragPos) {

    vec3 lightEnergy = light.color * light.intensity;// Total emission power
    vec3 lightDir = normalize(-light.direction);   // (Directional)
    vec3 ambient = (lightEnergy * 0.1) *  albedo;  // Ambient lighting
    float diff   = max(dot(normal, lightDir), 0.0);// Diffuse lighting
    vec3 diffuse = lightEnergy * diff * albedo;    // Diffuse lighting

    // Specular lighting
    vec3  reflectDir = reflect(-lightDir, normal);
    float spec       = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular    =  lightEnergy * spec * specMap;

    return ambient + diffuse + specular;
}

//----------------------------
//  POINT LIGHT CALCULATION  -
//----------------------------
vec3 calculatePointLight(PointLight light, vec3 normal, vec3 viewDir, vec3 albedo, vec3 specMap, vec3 fragPos) {

    vec3 lightEnergy = light.color * light.intensity; // Total emissive power
    vec3 lightDir = normalize(light.position - fragPos);// (Point/Spot)
    vec3 ambient = (lightEnergy * 0.1) * albedo;  // Ambient lighting
    float diff   = max(dot(normal, lightDir), 0.0);// Diffuse lighting
    vec3 diffuse = lightEnergy * diff * albedo;    // Diffuse lighting

    // Specular lighting
    vec3  reflectDir = reflect(-lightDir, normal);
    float spec       = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular    =  lightEnergy * spec * specMap;

    // Attenuation: intensity loss over distance
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    return (ambient + diffuse + specular) * attenuation;
}

//---------------------------
//  SPOT LIGHT CALCULATION  -
//---------------------------
vec3 calculateSpotLight(SpotLight   light, vec3 normal, vec3 viewDir, vec3 albedo, vec3 specMap, vec3 fragPos) {

    vec3 lightEnergy = light.color * light.intensity;   // Total emissive power
    vec3 lightDir = normalize(light.position - fragPos);// (Point/Spot)
    vec3 ambient = (lightEnergy * 0.1) *  albedo; // Ambient Lighting
    float diff = max(dot(normal, lightDir), 0.0); // Diffuse Lighting
    vec3 diffuse    = lightEnergy * diff * albedo;// Diffuse Lighting

    // Specular Lighting
    vec3  reflectDir = reflect(-lightDir, normal);
    float spec       = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular   =  lightEnergy * spec * specMap;

    // Soft spotlight intensity calculation
    vec3 spotDir  = normalize(light.direction);// Spotlight direction vector
    float theta   = dot(lightDir, -spotDir);   // scalar product
    float phi     = light.cutOff;              // cos(inner_angle)
    float epsilon = phi - light.outerCutOff;   // cosine difference: cos(in) - cos(out)
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);// Formula

    return (ambient + diffuse + specular) * intensity;
}
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
