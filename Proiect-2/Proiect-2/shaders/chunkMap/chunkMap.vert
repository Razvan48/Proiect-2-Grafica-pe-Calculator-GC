#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoord;

out vec2 texCoord;

out vec3 fragPosition;
out vec3 fragNormal;

uniform mat4 projection;
uniform mat4 view;

void main()
{
	texCoord = inTexCoord;

	fragPosition = inPosition;
	fragNormal = inNormal;

	gl_Position = projection * view * vec4(inPosition, 1.0);
}
