#version 330 core

in vec2 LocalCoord;
in vec4 Color;

out vec4 FragColor;

void main()
{
    float r2 = dot(LocalCoord, LocalCoord);

    if (r2 > 1.0) {
        discard;
    }

    float alpha = exp(-4.0 * r2) * Color.a;

    FragColor = vec4(Color.rgb, alpha);
}
