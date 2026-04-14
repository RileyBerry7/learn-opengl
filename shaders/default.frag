#version 330 core
out vec4 FragColor;

in vec3 color;
in vec2 texCoord;
in vec3 normal;

uniform sampler2D tex0;
uniform vec3 lightColor;

void main()
{
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;
    vec3 result  = ambient * color;

    FragColor = texture(tex0, texCoord) * vec4(result, 1.0);
}
