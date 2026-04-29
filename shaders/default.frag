#version 330 core

// Phong Lighting Functions
vec3 calculateDirectionalLight();

//----------------------------------------------------------------------------------------------------------------------
// Type Definitions
struct Material {
    sampler2D diffuse;   // diffuse map (texture)
    sampler2D specular;  // specuar map (texture)
    float     shininess; //
};
// In GLSL sizes are calculated based on 16-byte chunks. All elements reserve 16 bytes.
struct DirLight {    // Total: 32 bytes
    vec3  direction; // 12 bytes
    float padding;   // 4  bytes
    vec3  color;     // 12 bytes
    float intensity; // 4 bytes
};
struct PointLight {   // Total size: 48
    vec3  position;  // 12 bytes
    float padding;   // 4  bytes
    vec3  color;     // 12 bytes
    float intensity; // 4 bytes

    // Attenuation factors
    float constant;  // 4 bytes
    float linear;    // 4 bytes
    float quadratic; // 4 bytes
    float radius;    // 4 bytes
};
struct SpotLight {    // Total: 48 bytes
    vec3  position;   // 12 bytes
    float padding1;   // 4  bytes
    vec3  color;      // 12 bytes
    float padding2;   // 4  bytes
    float intensity;  // 4 bytes
    float cutOff;     // 4 bytes
    float outerCutOff;// 4 bytes
    float padding3;   // 4  bytes
};
// GLSL enum representation of LightType
const uint DIRECTIONAL = 0u;
const uint POINT       = 1u;
const uint SPOT        = 2u;
//----------------------------------------------------------------------------------------------------------------------

// Configuration
#define MAX_LIGHTS 10

// -- Light Uniforms Buffers --
layout (std140, binding = 0 ) uniform DirLightBlock {
    DirLight dirLights[MAX_LIGHTS];// Directional light list
};
layout (std140, binding = 0 ) uniform DirLightBlock {
    PointLight pointLights[MAX_LIGHTS];// Point light list
};
layout (std140, binding = 0 ) uniform DirLightBlock {
    SpotLight  spotLightsi[MAX_LIGHTS];// Spot light list
};
uniform int       dirCount;   // Directional light count
uniform int       pointCount; // Point light count
uniform int       spotCount;  // Spot light count

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

    // Light Loop
    for (int i = 0; i < lightCount; i++) {

        switch (lights[i].type){
//            case DIRECTIONAL: vec3 lightsDir = normalize(-lights[i].direction);// Directional Light
//            break;
//            case POINT: vec3  lightDir = normalize(lights[i].position - fragPos);// Point Light
//            // Point attenuation
//            float distance    = length(lights[i].position - fragPos);
//            float attenuation = 1.0 / (lights[i].constant + lights[i].linear * distance + lights[i].quadratic * (distance * distance));

            // break;
            case SPOT: vec3 lightDir = normalize(lights[i].position - fragPos);    // Spot
            // Spot light cone calcuation
            vec3 spotDir  = normalize(lights[i].direction);
            float theta   = dot(lightDir, -spotDir);
            float phi     = lights[i].cutOff; // cos(phi) is expensive so done on cpu
            float epsilon = phi - lights[i].outerCutOff; // cosine difference
            // Spot intensity
            float intensity = clamp((theta - lights[i].outerCutOff) / epsilon, 0.0, 1.0);
            // Ambient Lighting
            ambient = lights[i].ambient * texture(material.diffuse, texCoord).rgb;

            // Diffuse Lighting
            float diff     = max(dot(norm, lightDir), 0.0);
            diffuse  = lights[i].diffuse * diff * albedo;

            // Specular Lighting
            vec3  reflectDir = reflect(-lightDir, norm);
            float spec       = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
            float brightness = dot(specMap, vec3(0.2126, 0.7152, 0.0722));// Weighted brightness
            specular   =  lights[i].specular * spec * brightness;
            //          vec3 specular = lights[i].specular * spec * texture(material.specular, texCoord).rgb;

            // Apply intensity / Attenuation (Point/Spot)
            ambient  *= intensity;
            diffuse  *= intensity;
            specular *= intensity;
            break;
        }
        totalLight += ambient + diffuse + specular;
    }
    // Calculate Result
    result = totalLight * objColor;
    FragColor = vec4(result, 1.0);
}
//----------------------------------------------------------------------------------------------------------------------
