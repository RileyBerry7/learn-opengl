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
    // World-space position
    vec4 worldPos = modelMatrix * vec4(aPos, 1.0);
    fragPos = worldPos.xyz;

    // Final clip-space position
    gl_Position = camMatrix * worldPos;

    // Pass-through vertex data
    objColor = aColor;
    texCoord = aTex;

    // Build TBN matrix
    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
    vec3 N = normalize(normalMatrix * aNormal);
    vec3 T = normalize(normalMatrix * aTangent);
    T = normalize(T - dot(T, N) * N); // Re-orthogonalize tangent against normal
    vec3 B = normalize(cross(N, T)); // Construct bitangent
    TBN    = mat3(T, B, N); // Final basis
    normal = N;
}
//======================================================================================================================
