#version 330 core

//----------------------------------------------------------------------------------------------------------------------
// Type Definitions
struct Material {
    sampler2D diffuse;   // diffuse map (texture)
    sampler2D specular;  // specuar map (texture)
    float     shininess; //
};
struct Light {
    uint  type;      // Directional/Point/Spot
    vec3  ambient;   // Directional/Point/Spot
    vec3  diffuse;   // Directional/Point/Spot
    vec3  specular;  // Directional/Point/Spot
    vec3  direction; // Directional
    float constant;  // Point
    float linear;    // Point
    float quadratic; // Point
    vec3  position;  // Point/Spot
    float cutOff;    // Spot
};
// GLSL enum representation of LightType
const uint DIRECTIONAL_LIGHT = 0u;
const uint POINT_LIGHT       = 1u;
const uint SPOT_LIGHT        = 2u;
//----------------------------------------------------------------------------------------------------------------------

// Configuration
#define MAX_LIGHTS 10

// Uniforms
uniform Light     lights[MAX_LIGHTS]; // Light
uniform int       lightCount;         // Light
uniform sampler2D tex0;     // Fragment
uniform vec3      viewPos;  // Fragment
uniform Material  material; // Fragment

// Input
in vec3 objColor;
in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;

// Output
out vec4 FragColor;


//======================================================================================================================
void main()
{
    vec3 totalAmbient  = vec3(0.0); // accumulates ambient color
    vec3 totalDiffuse  = vec3(0.0); // accumulates diffuse color
    vec3 totalSpecular = vec3(0.0); // accumulates specular color

    // Light Loop
    for (int i = 0; i < MAX_LIGHTS; i++) {
        if (i == lightCount) break;
        if (i != 2) continue;
        // Spot light
        switch (lights[i].type){
            case SPOT_LIGHT:
            vec3 lightDir = normalize(lights[i].position - fragPos);
            vec3 spotDir  = normalize(lights[i].direction);
            float theta   = dot(lightDir, -spotDir);
            float phi     = lights[i].cutOff;
            if (theta > phi){

                // Ambient Lighting
                vec3 ambient = lights[i].ambient * texture(material.diffuse, texCoord).rgb;
                // Diffuse Lighting
                vec3  norm     =     normalize(normal);
                float diff     = max(dot(norm, lightDir), 0.0);
                vec3  diffuse  = lights[i].diffuse * diff * texture(material.diffuse, texCoord).rgb;
                // Specular Lighting
                vec3  viewDir    = normalize(viewPos -fragPos);
                vec3  reflectDir = reflect(-lightDir, norm);
                float spec       = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
                float brightness = dot(texture(material.specular, texCoord).rgb, vec3(0.2126, 0.7152, 0.0722));// Weighted brightness
                vec3  specular   =  lights[i].specular * spec * brightness;
                // Acuumulate colors
                totalAmbient  += ambient;
                totalDiffuse  += diffuse;
                totalSpecular += specular;
            }
            break;
        }

//            case POINT_LIGHT:
//            // Ambient Lighting
//            vec3 ambient = lights[i].ambient * texture(material.diffuse, texCoord).rgb;
//
//            // Diffuse Lighting
//            vec3  norm     =     normalize(normal);
//            vec3  lightDir = normalize(lights[i].position - fragPos);// Point Light
//            //    vec3 lightsDir  = normalize(-lights[i].direction);       // Directional Light
//            float diff     = max(dot(norm, lightDir), 0.0);
//            vec3  diffuse  = lights[i].diffuse * diff * texture(material.diffuse, texCoord).rgb;
//
//            // Specular Lighting
//            vec3  viewDir    = normalize(viewPos -fragPos);
//            vec3  reflectDir = reflect(-lightDir, norm);
//            float spec       = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
//            float brightness = dot(texture(material.specular, texCoord).rgb, vec3(0.2126, 0.7152, 0.0722));// Weighted brightness
//            vec3  specular   =  lights[i].specular * spec * brightness;
//            // standard approach
////            vec3 specular = lights[i].specular * spec * texture(material.specular, texCoord).rgb;
//
//            // Attenuation Calculation (Point Light)
//            float distance    = length(lights[i].position - fragPos);
//            float attenuation = 1.0 / (lights[i].constant + lights[i].linear * distance + lights[i].quadratic * (distance * distance));
//
//            // Attenuation Application (Point Light)
//            ambient  *= attenuation;
//            diffuse  *= attenuation;
//            specular *= attenuation;
//            break;
        //}
            // Acuumulate colors
//            totalAmbient  += ambient;
//            totalDiffuse  += diffuse;
//            totalSpecular += specular;
    }
    // Calculate Result
    vec3 result = (totalAmbient + totalDiffuse + totalSpecular) * objColor;

    FragColor = vec4(result, 1.0);
}
//----------------------------------------------------------------------------------------------------------------------
