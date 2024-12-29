#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inTexCoord;

out vec2 texCoord;

uniform mat4 projection;
uniform mat4 view;

uniform vec4 plane; // = vec4(0, -1, 0, 5.1);

void main()
{
	vec4 worldPosition = vec4(inPosition, 1.0);
	gl_ClipDistance[0] = dot(worldPosition, plane);

	texCoord = inTexCoord;
	gl_Position = projection * view * vec4(inPosition, 1.0);
}
