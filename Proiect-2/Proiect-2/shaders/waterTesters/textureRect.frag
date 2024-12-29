#version 330 core

in vec2 vertOutTexCoords;
in vec3 vertOutColor;

out vec4 out_Color;

uniform sampler2D ourTexture;

void main()
{
    // out_Color = vec4(1.0f, 1.0f, 0.0f, 1.0);
    // out_Color = vec4(vertOutColor, 1.0);
    out_Color = texture(ourTexture, vertOutTexCoords);
}