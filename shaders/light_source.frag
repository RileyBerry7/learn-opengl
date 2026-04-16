#version 330 core

out vec4 FragColor;

in vec3 objColor;
in vec3 fragPos;

void main()
{
    FragColor = vec4(objColor, 1.0);
}
