#version 330 core


struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float     shininess;
};
struct Light {
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

out vec4 FragColor;

in vec3 objColor;
in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;

#define MAX_LIGHTS 10
uniform Light lights[MAX_LIGHTS];
uniform int   lightCount;
uniform sampler2D tex0;
uniform vec3      viewPos;
uniform Material  material;

void main()
{
    vec3 totalAmbient  = vec3(0.0);
    vec3 totalDiffuse  = vec3(0.0);
    vec3 totalSpecular = vec3(0.0);

    for (int i = 0; i < MAX_LIGHTS; i++) {
        if (i == lightCount) break;

        // Attenuation Calculation (Point Light)
        float distance    = length(lights[i].position - fragPos);
        float attenuation = 1.0 / (lights[i].constant + lights[i].linear * distance + lights[i].quadratic * (distance * distance));

        // Ambient Lighting
        vec3 ambient = lights[i].ambient * texture(material.diffuse, texCoord).rgb;

        // Diffuse Lighting
        vec3  norm     =     normalize(normal);
        vec3  lightDir = normalize(lights[i].position - fragPos);// Point Light
        //    vec3 lightsDir  = normalize(-lights[i].direction);       // Directional Light
        float diff     = max(dot(norm, lightDir), 0.0);
        vec3  diffuse  = lights[i].diffuse * diff * texture(material.diffuse, texCoord).rgb;

        // Specular Lighting
        //    float specularStrength = 0.6;
        vec3  viewDir    = normalize(viewPos -fragPos);
        vec3  reflectDir = reflect(-lightDir, norm);
        float spec       = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        float brightness = dot(texture(material.specular, texCoord).rgb, vec3(0.2126, 0.7152, 0.0722));// Weighted brightness
        vec3  specular   =  lights[i].specular * spec * brightness;

        // Attenuation Application (Point Light)
        ambient  *= attenuation;
        diffuse  *= attenuation;
        specular *= attenuation;

        totalAmbient  += ambient;
        totalDiffuse  += diffuse;
        totalSpecular += specular;
    }
    // Calculate Result
    vec3 result = (totalAmbient + totalDiffuse + totalSpecular) * objColor;

    FragColor = vec4(result, 1.0);
}
