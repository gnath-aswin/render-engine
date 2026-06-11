#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in float aOpacity;
layout (location = 2) in vec3 aScale;
layout (location = 3) in vec4 aColor;

out vec4 Color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float pointSizeMultiplier;

void main()
{
    vec4 worldPos = model * vec4(aPos, 1.0);
    vec4 viewPos = view * worldPos;

    float depth = max(0.1, -viewPos.z);
    float size = max(aScale.x, max(aScale.y, aScale.z));

    gl_PointSize = clamp(
        (size * pointSizeMultiplier) / depth,
        1.0,
        12.0
    );

    gl_Position = projection * viewPos; // I dont know output to the fragment shader must be at the end, I got error with setting uniforms

    Color = vec4(aColor.rgb, aOpacity);
}
