#version 330 core
out vec4 FragColor;

in vec3 color;
in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;

uniform sampler2D tex0;
uniform vec3 lightColor;
uniform vec3 lightPos;

void main()
{
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;
//    vec3 result  = ambient * color;

    vec3  norm     = normalize(normal);
    vec3  lightDir = normalize(lightPos - fragPos);
    float diff     = max(dot(norm, lightDir), 0.0);
    vec3  diffuse  = diff * lightColor;

    vec3 result = (ambient + diffuse) * color;

    FragColor = texture(tex0, texCoord) * vec4(result, 1.0);
}
