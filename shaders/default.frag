#version 330 core


struct Material {
//    vec3 ambient; We dont need this anymore since ambient is stored decided by the light struct
//    vec3 diffuse;
    sampler2D diffuse;
    vec3      specular;
    float     shininess;
};
struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec4 FragColor;

in vec3 objColor;
in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;

uniform sampler2D tex0;
//uniform vec3 lightColor;
//uniform vec3 lightPos;
uniform vec3 viewPos;
uniform Material material;
uniform Light light;

// Orange-Bronze
//Material material = Material(vec3(1.0, 0.5, 0.31), vec3(1.0, 0.5, 0.31), vec3(0.5, 0.5, 0.5), 32.0);
// Point-Light
//Light    light    = Light(lightPos, vec3(0.2, 0.2, 0.2), vec3(0.5, 0.5, 0.5), vec3(1.0, 1.0, 1.0));

void main()
{
    // Ambient Lighting
//    float ambientStrength = 0.2;
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoord));

    // Diffuse Lighting
    vec3  norm     = normalize(normal);
    vec3  lightDir = normalize(light.position - fragPos);
    float diff     = max(dot(norm, lightDir), 0.0);
    vec3  diffuse  = light.diffuse * diff * vec3(texture(material.diffuse, texCoord));
    //    vec3  diffuse  = light.diffuse * (diff * material.diffuse);


    // Specular Lighting
//    float specularStrength = 0.6;
    vec3  viewDir   = normalize(viewPos -fragPos);
    vec3  reflectDir = reflect(-lightDir, norm);
    float spec      = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3  specular  =  light.specular * (spec * material.specular);

    // Calculate Result
    vec3 result = (ambient + diffuse + specular) * objColor;

    FragColor = vec4(result, 1.0);
}
