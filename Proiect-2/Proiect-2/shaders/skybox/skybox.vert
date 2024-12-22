#version 330 core

layout (location = 0) in vec3 inPosition;

out vec3 fragTexCoord;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    fragTexCoord = inPosition;
    vec4 pos = projection * view * vec4(inPosition, 1.0);
    gl_Position = pos.xyww;
}
