#version 420 core
//----------------------------------------------------------------------------------------------------------------------
// UNIFORMS
uniform mat4 camMatrix;
uniform mat4 modelMatrix;
//----------------------------------------------------------------------------------------------------------------------
// INPUT - vertex attributes from VBO / VAO
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTex;
layout (location = 3) in vec3 aNormal;
layout  (location = 4) in vec3 aTangent;
//----------------------------------------------------------------------------------------------------------------------
// OUTPUT
out vec3 objColor;
out vec2 texCoord;
out vec3 normal;
out vec3 fragPos;
out mat3 TBN;
//======================================================================================================================
void main()
{
    // Apply scale and camera matrix
    gl_Position = camMatrix * modelMatrix * vec4(aPos, 1.0);
    objColor    = aColor;
    texCoord    = aTex;

    // The Normal Matrix: inverse transpose of the 3x3 model matrix
    normal = mat3(transpose(inverse(modelMatrix))) * aNormal;
    fragPos = vec3(modelMatrix * vec4(aPos, 1.0));

    // TBN matrix calculation
    vec3 T = normalize(vec3(modelMatrix * vec4(aTangent, 0,0)));
    vec3 N = normalize(vec3(modelMatrix * vec4(aNormal, 0.0)));
    vec3 B = cross(N, T);
    TBN = mat3(T, B, N);

}
//======================================================================================================================
