#version 330 core

layout (location=0) in vec3 inPosition;
layout (location=1) in vec3 inNormal;
layout (location=2) in vec2 inTexCoord;

out vec2 fragTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(void)
{
    fragTexCoord = vec2(inTexCoord.x, 1.0 - inTexCoord.y);
    gl_Position = projection * view * model * vec4(inPosition, 1.0);
}
