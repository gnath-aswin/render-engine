#version 330 core

in vec2 EllipseCoord;
in vec2 GaussianCoord;
in vec4 Color;

out vec4 FragColor;

void main()
{
    // Keep only ellipse, not the full square quad.
    if (dot(EllipseCoord, EllipseCoord) > 1.0) {
        discard;
    }

    // Gaussian falloff: exp(-0.5 * r^2)
    float r2 = dot(GaussianCoord, GaussianCoord);
    float alpha = exp(-0.5 * r2) * Color.a;

    if (alpha < 0.003) {
        discard;
    }

    FragColor = vec4(Color.rgb, alpha);
}
