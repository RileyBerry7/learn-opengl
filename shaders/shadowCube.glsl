//======================================================================================================================
#type vertex
#version 430 core
#extension GL_ARB_shader_viewport_layer_array : require
#include "lightUBO.glsl.h"

//======================================================================================================================
layout (location = 0) in vec3 aPos;
uniform mat4 modelMatrix;
uniform int  cubeMapIndex;
out vec3     fragPos;
flat out vec3  lightPos;  // The world-space position of the point light
flat out float farPlane;  // The far clipping plane of the light's projection matrix

void main() {

    int lightIndex   = cubeMapIndex;  // Light index
    int faceIndex    = gl_InstanceID; // Face  index
    gl_Layer         = (6 * lightIndex) + faceIndex;
    vec4 worldPos    = modelMatrix * vec4(aPos, 1.0); //
    PointLight light = pointLights[lightIndex];       //

    mat4 lightSpaceMatrix;
    switch(faceIndex){
        case 0: lightSpaceMatrix = light.rightLSM;
                break;
        case 1: lightSpaceMatrix = light.leftLSM;
                break;
        case 2: lightSpaceMatrix = light.topLSM;
                break;
        case 3: lightSpaceMatrix = light.bottomLSM;
                break;
        case 5: lightSpaceMatrix = light.frontLSM; // TODO: front/back calculations are swapped, pls fix
                break;
        case 4: lightSpaceMatrix = light.backLSM;
                break;
    }
    // Compute final position using the specific face matrix
    gl_Position = lightSpaceMatrix * worldPos;
    fragPos  = vec3(worldPos);
    lightPos = light.position;
    farPlane = light.radius;
}
//----------------------------------------------------------------------------------------------------------------------



//======================================================================================================================
#type fragment
#version 430 core

in vec3 fragPos; // We need the world-space position from the vertex shader
flat in vec3 lightPos;   // The world-space position of the point light
flat in float farPlane;  // The far clipping plane of the light's projection matrix

void main() {
    float lightDistance = length(fragPos - lightPos); // Distance (light -> fragment)

    // Map to [0, 1] range by dividing by farPlane
    lightDistance = lightDistance / farPlane;

    // Write this as the explicit depth value
    gl_FragDepth = lightDistance;
}
//----------------------------------------------------------------------------------------------------------------------
