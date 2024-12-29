#version 330 core

layout (location = 0) in vec3 inPosition;

out vec3 fragTexCoord;

uniform mat4 projection;
uniform mat4 view;

// uniform vec4 plane; // = vec4(0, -1, 0, 5.1);

void main()
{
    // vec4 worldPosition = vec4(inPosition, 1.0);
	// gl_ClipDistance[0] = dot(worldPosition, plane);

    fragTexCoord = inPosition;
    vec4 pos = projection * view * vec4(inPosition, 1.0);
    gl_Position = pos.xyww;
}
