#version 330 core

in vec4 Color;
out vec4 FragColor;

void main()
{
    vec2 coord = gl_PointCoord * 2.0 - 1.0;
    float r2 = dot(coord, coord);

    if (r2 > 1.0) {
        discard;
    }

    float alpha = exp(-4.0 * r2) * Color.a;

    FragColor = vec4(Color.rgb, alpha);
}
