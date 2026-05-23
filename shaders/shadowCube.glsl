//======================================================================================================================
#type vertex
#version 430 core
#extension GL_ARB_shader_viewport_layer_array : require
// Upgraded to 4.3 for native ARB_shader_viewport_layer_array

layout (location = 0) in vec3 aPos;

uniform mat4 modelMatrix;
uniform mat4 lightSpaceMatrices[20]; // Combined Max Dir + Spot lights

void main() {
    // 1. Calculate the layer index directly from the instance ID
    int layer = gl_InstanceID;

    // 2. Set the hardware destination layer directly from the vertex shader!
    gl_Layer = layer;

    // 3. Compute final position immediately
    gl_Position = lightSpaceMatrices[layer] * modelMatrix * vec4(aPos, 1.0);
}

//----------------------------------------------------------------------------------------------------------------------

//======================================================================================================================
#type fragment
#version 430 core
//======================================================================================================================
void main()
{

}
//----------------------------------------------------------------------------------------------------------------------
