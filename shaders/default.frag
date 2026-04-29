#version 420 core

//----------------------------------------------------------------------------------------------------------------------
// Type Definitions
struct Material {
    sampler2D diffuse;   // diffuse map (texture)
    sampler2D specular;  // specuar map (texture)
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

// Configuration
#define MAX_LIGHTS 10

// -- Light Uniform Buffer --
layout (std140, binding = 0 ) uniform LightData
{
    DirLight   dirLights[MAX_LIGHTS];
    PointLight pointLights[MAX_LIGHTS];
    SpotLight  spotLights[MAX_LIGHTS];
    int dirCount, pointCount, spotCount;
};

uniform sampler2D tex0;     // Texture uniform
uniform vec3      viewPos;  // View position uniform
uniform Material  material; // Material uniform

// Input <- Rasterizer <- vertex_shader
in vec3 objColor;
in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;

// Output -> Per-Pixes Tests -> Blending -> Frame Buffer
out vec4 FragColor;


//======================================================================================================================
void main()
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 result = vec3(0.0);
    vec3 totalLight = vec3(0.0);

    // Calculate these once outside main
    vec3 norm     = normalize(normal);
    vec3  viewDir = normalize(viewPos -fragPos);
    vec3 albedo   = texture(material.diffuse, texCoord).rgb;
    vec3 specMap  = texture(material.specular, texCoord).rgb;

    // Spot Light Loop
    for (int i = 0; i < spotCount; i++) {

//            vec3 lightsDir = normalize(-lights[i].direction);// Directional Light
//            lightDir = normalize(lights[i].position - fragPos);// Point Light
//            // Point attenuation
//            float distance    = length(lights[i].position - fragPos);
//            float attenuation = 1.0 / (lights[i].constant + lights[i].linear * distance + lights[i].quadratic * (distance * distance));
        vec3 lightDir = (spotLights[i].position - fragPos);

        // Spot light cone calcuation
        vec3 spotDir  = normalize(spotLights[i].direction);
        float theta   = dot(lightDir, -spotDir);
        float phi     = spotLights[i].cutOff; // cos(phi) is expensive so done on cpu
        float epsilon = phi - spotLights[i].outerCutOff; // cosine difference
        // Spot intensity
        float intensity = clamp((theta - spotLights[i].outerCutOff) / epsilon, 0.0, 1.0);

        vec3 lightEnergy = spotLights[i].color * spotLights[i].intensity;

        // Ambient Lighting
        ambient = (lightEnergy * 0.1) *  albedo;

        // Diffuse Lighting
        float diff = max(dot(norm, lightDir), 0.0);
        diffuse    = lightEnergy * diff * albedo;

        // Specular Lighting
        vec3  reflectDir = reflect(-lightDir, norm);
        float spec       = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        float brightness = dot(specMap, vec3(0.2126, 0.7152, 0.0722));// Weighted brightness
        specular   =  lightEnergy * spec * brightness;
        //          vec3 specular = lightEnergy * spec * specMap;

        // Apply intensity / Attenuation (Point/Spot)
        ambient  *= intensity;
        diffuse  *= intensity;
        specular *= intensity;
        totalLight += ambient + diffuse + specular;
    }
    // Calculate Result
    result = totalLight * objColor;
    FragColor = vec4(result, 1.0);
}
//----------------------------------------------------------------------------------------------------------------------
