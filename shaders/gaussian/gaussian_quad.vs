#version 330 core

layout (location = 0) in vec2 aCorner;

layout (location = 1) in vec3 aPos;
layout (location = 2) in float aOpacity;
layout (location = 3) in vec3 aScale;
layout (location = 4) in vec4 aRotation;
layout (location = 5) in vec4 aColor;

out vec2 LocalCoord;
out vec4 Color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float splatSizeMultiplier;

void main()
{
    vec4 centerView = view * model * vec4(aPos, 1.0);

    float depth = max(0.1, -centerView.z);

    float sx = aScale.x * splatSizeMultiplier / depth;
    float sy = aScale.y * splatSizeMultiplier / depth;

    sx = clamp(sx, 0.0001, 0.01);
    sy = clamp(sy, 0.0001, 0.01);

    vec4 offsetView = vec4(aCorner.x * sx, aCorner.y * sy, 0.0, 0.0);

    gl_Position = projection * (centerView + offsetView);

    LocalCoord = aCorner;
    Color = vec4(aColor.rgb, aOpacity);
}
