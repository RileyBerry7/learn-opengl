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

uniform sampler2D tex0;
uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
    // Attenuation Calculation (Point Light)
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // Ambient Lighting
    vec3 ambient = light.ambient * texture(material.diffuse, texCoord).rgb;

    // Diffuse Lighting
    vec3  norm     =     normalize(normal);
    vec3  lightDir = normalize(light.position - fragPos); // Point Light
//    vec3 lightDir  = normalize(-light.direction);       // Directional Light
    float diff     = max(dot(norm, lightDir), 0.0);
    vec3  diffuse  = light.diffuse * diff * texture(material.diffuse, texCoord).rgb;

    // Specular Lighting
//    float specularStrength = 0.6;
    vec3  viewDir   = normalize(viewPos -fragPos);
    vec3  reflectDir = reflect(-lightDir, norm);
    float spec      = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    float brightness = dot(texture(material.specular, texCoord).rgb, vec3(0.2126, 0.7152, 0.0722)); // Weighted brightness
    vec3  specular  =  light.specular * spec * brightness;

    // Attenuation Application (Point Light)
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    // Calculate Result
    vec3 result = (ambient + diffuse + specular) * objColor;

    FragColor = vec4(result, 1.0);
}
