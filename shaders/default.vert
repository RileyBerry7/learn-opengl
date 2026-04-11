#version 330 core

// Vertex attributes from VBO / VAO
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTex;

out vec3 color;
out vec2 texCoord;

uniform mat4 camMatrix;
uniform mat4 modelMatrix;

void main()
{
    // Rotate 90 degrees on Z: (x, y) becomes (-y, x)
    vec3 rotatedPos = vec3(aPos.x, aPos.z, -aPos.y);

    // Apply scale and camera matrix
    gl_Position = camMatrix * modelMatrix * vec4(rotatedPos*(0.001), 1.0);
    color = aColor;
    texCoord = aTex;
}

