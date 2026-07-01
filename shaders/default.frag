#version 420 core
#include "lightUBO.glsl.h"

//----------------------------------------------------------------------------------------------------------------------
// Global Uniforms
layout(binding = 4) uniform sampler2DArray   shadowArray2D;
layout(binding = 5) uniform samplerCubeArray shadowArrayCube;
//----------------------------------------------------------------------------------------------------------------------
// UNIFORMS
uniform vec3      viewPos;  // View position
uniform bool      toggleF;  // Flashlight control
uniform Material  material; // Material
//----------------------------------------------------------------------------------------------------------------------
// INPUT
in vec3 objColor;
in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;
in mat3 TBN;
//----------------------------------------------------------------------------------------------------------------------
// OUTPUT
out vec4 FragColor;
//----------------------------------------------------------------------------------------------------------------------
// FUNCTION PROTOTYPES
vec3 calculateDirLight(DirLight   light, vec3 normal, vec3 viewDir, vec3 specMap);
vec3 calculatePointLight(PointLight   light, vec3 normal, vec3 viewDir, vec3 specMap, vec3 fragPos);
vec3 calculateSpotLight(SpotLight   light, vec3 normal, vec3 viewDir, vec3 specMap, vec3 fragPos);
vec3 calcSpecular(vec3 normal, vec3 lightDir, vec3 viewDir, float materialShine, vec3 lightEnergy, vec3 specMap, vec3 specScalar);
float calculateShadow2D(vec3 lightDir, vec3 normal, mat4 lightSpaceMatrix, int shadowId);
float calculateShadowCube(vec3 fragPos, vec3 normal, int shadowId, vec3 lightPos, float lightRadius);
//======================================================================================================================
void main()
{

    vec3 totalLight = vec3(0.0); // Light color accumulator

    // Calculate loop invariants

    // NORMAL MAPPING
    vec3 norm;
    if (material.hasNormalMap){
        vec3 normalMap = texture(material.normalMap, texCoord).rgb;
        norm = normalize(normalMap * 2.0 - 1.0);
        norm = TBN * norm;

    } else {
        norm      = normalize(normal);
    }

    // PARALLAX MAPPING
    vec3 viewDir  = normalize(viewPos -fragPos);
    vec2 UVs = texCoord;
    if (material.hasDispMap) {

        vec3 viewDirTS = normalize(transpose(TBN) * viewDir);

        // Parallax occlusion mapping quality
        float heightScale = 0.05;
        const float minLayers = 8.0;
        const float maxLayers = 64.0;
        float numLayers  = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDirTS)));
        float layerDepth = 1 / numLayers;
        float currLayerDepth = 0.0;

        // Remove z division
        vec2 S = viewDirTS.xy / viewDirTS.z * heightScale;
        vec2 deltaUVs = S / numLayers;

        float currDepthMapValue = 1.0 - texture(material.dispMap, UVs).r;

        // Loop point on the heightmap is hit
        while (currLayerDepth < currDepthMapValue){
            UVs -= deltaUVs;
            currDepthMapValue = 1.0 - texture(material.dispMap, UVs).r;
            currLayerDepth += layerDepth;
        }
        // Apply occlusion (interpolation with prev value)
        vec2 prevTexCoords = UVs + deltaUVs;
        float afterDepth   = currDepthMapValue - currLayerDepth;
        float beforeDepth  = 1.0 - texture(material.dispMap, prevTexCoords).r - currLayerDepth + layerDepth;
        float weight = afterDepth / (afterDepth - beforeDepth);
        UVs =   prevTexCoords * weight + UVs * (1.0 - weight);

        // Discard fragments outside of texture range
        if (UVs.x > 1.0 || UVs.y > 1.0 || UVs.x < 0.0 || UVs.y < 0.0){
            discard;
        }
    }

    vec3 albedo   = pow(texture(material.diffuseMap, UVs).rgb, vec3(2.2));//remove gamma from texture
    vec3 specMap  = texture(material.specularMap, UVs).rgb;
    vec3 ambient  = albedo * 0.05;

    // Calculate total light
    for (int i = 0; i < dirCount; i++)
        totalLight += calculateDirLight(dirLights[i], norm, viewDir, specMap);
    for (int i = 0; i < pointCount; i++)
        totalLight += calculatePointLight(pointLights[i], norm, viewDir, specMap, fragPos);
    for (int i = 0; i < spotCount; i++){
        if (!toggleF && i == 0) continue;
        totalLight += calculateSpotLight(spotLights[i], norm, viewDir, specMap, fragPos);
    }
    vec3 finalColor = (totalLight + ambient) * albedo * objColor;// Combine color components
    finalColor = finalColor / (finalColor + vec3(1.0));// Reinhard tone mapping
    finalColor = pow(finalColor, vec3(1.0/2.2));       // Gamma correction
    FragColor  = vec4(finalColor, 1.0);                 // Append alpha channel
}
//===================================V===================================================================================

//----------------------------------
//  DIRECTIONAL LIGHT CALCULATION  -
//----------------------------------
vec3 calculateDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 specMap) {

    vec3 lightEnergy = light.color * light.intensity;      // Total emission power
    vec3 lightDir = normalize(-light.direction);           // Direction->Light vector
    float diff    = max(dot(normal, lightDir), 0.0);       // Diffuse lighting
    vec3 diffuse  = lightEnergy * diff * material.diffuse; // Diffuse lighting
    // Specular lighting
    vec3 specular = calcSpecular(normal, lightDir, viewDir, material.shininess, lightEnergy, specMap, material.specular);
    float shadow = calculateShadow2D(lightDir, normal, light.lightSpaceMatrix, light.shadowId);       // Shadow mapping
    return (1.0 - shadow) * (diffuse + specular);
}

//----------------------------
//  POINT LIGHT CALCULATION  -
//----------------------------
vec3 calculatePointLight(PointLight light, vec3 normal, vec3 viewDir, vec3 specMap, vec3 fragPos) {

    vec3 lightEnergy = light.color * light.intensity;      // Total emissive power
    vec3 lightDir = normalize(light.position - fragPos);   // Frag->Light vector
    float diff    = max(dot(normal, lightDir), 0.0);       // Diffuse lighting
    vec3 diffuse  = lightEnergy * diff * material.diffuse; // Diffuse lighting
    // Specular lighting
    vec3 specular = calcSpecular(normal, lightDir, viewDir, material.shininess, lightEnergy, specMap, material.specular);

    // Attenuation: intensity loss over distance
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // Shadow
    float shadow = calculateShadowCube(fragPos, normal, light.shadowId, light.position, light.radius);

    return (1 - shadow) * (diffuse + specular) * attenuation;
}

//---------------------------
//  SPOT LIGHT CALCULATION  -
//---------------------------
vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 viewDir, vec3 specMap, vec3 fragPos) {

    vec3 lightEnergy = light.color * light.intensity;      // Total emissive power
    vec3 lightDir = normalize(light.position - fragPos);   // Frag->Light vector
    float diff    = max(dot(normal, lightDir), 0.0);       // Diffuse lighting
    vec3 diffuse  = lightEnergy * diff * material.diffuse; // Diffuse lighting
    // Specular lighting
    vec3 specular = calcSpecular(normal, lightDir, viewDir, material.shininess, lightEnergy, specMap, material.specular);

    // Soft spotlight intensity calculation
    vec3 spotDir  = normalize(light.direction);// Spotlight direction vector
    float theta   = dot(lightDir, -spotDir);   // scalar product
    float phi     = light.cutOff;              // cos(inner_angle)
    float epsilon = phi - light.outerCutOff;   // cosine difference: cos(in) - cos(out)
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);// Formula

    // Shadows
    vec4 lp = light.lightSpaceMatrix * vec4(fragPos, 1.0);
    vec3 proj = lp.xyz / lp.w * 0.5 + 0.5;

    float shadow = 1.0;

    if (proj.x >= 0.0 && proj.x <= 1.0 &&
    proj.y >= 0.0 && proj.y <= 1.0 &&
    proj.z >= 0.0 && proj.z <= 1.0)
    {
        float closest = texture(shadowArray2D, vec3(proj.xy, float(light.shadowId))).r;
        float bias = max(0.0015 * (1.0 - dot(normal, lightDir)), 0.0005);
        shadow = (proj.z - bias > closest) ? 1.0 : 0.0;
    }
    return (1.0 - shadow) * (diffuse + specular) * intensity;
}

//------------------------
// SPECULAR CALCULATION  -
//------------------------
vec3 calcSpecular(vec3 normal, vec3 lightDir, vec3 viewDir, float materialShine, vec3 lightEnergy, vec3 specMap, vec3 specScalar)
{
    float  energyConservation = (materialShine   + 2.0) / 8.0;// Scalar that increases as specular area decreases
    vec3   reflectDir = reflect(-lightDir, normal); // Frag->Reflection vector
    float  spec       = pow(max(dot(viewDir, reflectDir), 0.0), materialShine);
    return lightEnergy * spec * specMap * energyConservation * specScalar;
}
//------------------------
// SHADOW 2D CALCULATION  -
//------------------------
float calculateShadow2D(vec3 lightDir, vec3 normal, mat4 lightSpaceMatrix, int shadowId)
{
    vec4 fragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0);
    vec3 proj              = fragPosLightSpace.xyz / fragPosLightSpace.w * 0.5 + 0.5;
    if (proj.z > 1.0) return 0.0; // Prevent out-of-bounds over-shadowing
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005); // Stop shadow acne
    float shadow = (proj.z - bias > texture(shadowArray2D, vec3(proj.xy, float(shadowId))).r) ? 1.0 : 0.0;
    return shadow;
}
//-------------------------------
// SHADOW CUBE MAP CALCULATION  -
//-------------------------------
float calculateShadowCube(vec3 fragPos, vec3 normal, int shadowId, vec3 lightPos, float lightRadius)
{
    // Inside your MAIN lighting pass fragment shader:
    vec3 fragToLight   = fragPos - lightPos;
    float currentDepth = length(fragToLight) / lightRadius; // Matches shadow shader linear math

    // Sample the custom linear depth from the cubemaps array
    vec4  sampleCoords = vec4(normalize(fragToLight), float(shadowId));
    float closestDepth = texture(shadowArrayCube, sampleCoords).r;

    // Apply a tiny bias to prevent self-shadowing acne
    float bias = 0.005;
    if(currentDepth - bias > closestDepth) {
        // TRAPPED IN SHADOW (Pitch Black)
        return 1.0;
    } else {
        // ILLUMINATED (Apply colors/specular)
        return 0.0;
    }
}
//======================================================================================================================
