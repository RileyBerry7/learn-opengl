#version 330 core

// Vertex attributes from VBO / VAO
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTex;
layout (location = 3) in vec3 aNormal;

out vec3 objColor;
out vec2 texCoord;
out vec3 normal;
out vec3 fragPos;

uniform mat4 camMatrix;
uniform mat4 modelMatrix;

void main()
{
    // Apply scale and camera matrix
    gl_Position = camMatrix * modelMatrix * vec4(aPos, 1.0);
    objColor       = aColor;
    texCoord    = aTex;

    // The Normal Matrix: inverse transpose of the 3x3 model matrix
    normal = mat3(transpose(inverse(modelMatrix))) * aNormal;
//    normal  = aNormal;
    fragPos = vec3(modelMatrix * vec4(aPos, 1.0));
}