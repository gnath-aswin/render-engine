#version 330 core

out vec4 FragColor;

in vec2 TexCoord;
flat in int TexIndex;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;
uniform sampler2D texture4;
uniform sampler2D texture5;
uniform sampler2D texture6;

void main()
{
    if (TexIndex == 0)
        FragColor = texture(texture1, TexCoord);
    else if (TexIndex == 1)
        FragColor = texture(texture2, TexCoord);
    else if (TexIndex == 2)
        FragColor = texture(texture3, TexCoord);
    else if (TexIndex == 3)
        FragColor = texture(texture4, TexCoord);
    else if (TexIndex == 4)
        FragColor = texture(texture5, TexCoord);
    else
        FragColor = texture(texture6, TexCoord);
}
