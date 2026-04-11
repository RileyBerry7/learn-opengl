#version 330 core
out vec4 FragColor;

in vec3 color;
in vec2 texCoord;

uniform sampler2D tex0;

void main()
{
//    // Normalize screen position into 0–1 range
//    vec2 uv = gl_FragCoord.xy / vec2(800.0, 600.0);
//
//    // Simple pseudo-random function based on fragment position
//    float noise = fract(sin(dot(gl_FragCoord.xy, vec2(12.9898, 78.233))) * 43758.5453);
//
//    // Gradient colors
//    vec3 color = vec3(
//    uv.x,                  // red increases left -> right
//    uv.y,                  // green increases bottom -> top
//    0.5 + 0.5 * noise      // blue has random variation
//    );
//
//    FragColor = vec4(color, 1.0);
    FragColor = texture(tex0, texCoord);
}
