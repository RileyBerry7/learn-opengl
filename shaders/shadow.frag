#version 330 core
in vec4  FragPos;

uniform vec3 lightPot;
uniform float farPlane; // Max light range
void main()

{
    float lightDistance = length(FragPos.xyz - lightPot);
    lightDistance = lightDistance / farPlane;
    gl_FragDepth  = lightDistance;

}