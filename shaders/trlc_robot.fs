#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 baseColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    vec3 ambient = 0.4 * baseColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = 1.4 * diff * baseColor;

    FragColor = vec4(ambient + diffuse, 1.0);
}
