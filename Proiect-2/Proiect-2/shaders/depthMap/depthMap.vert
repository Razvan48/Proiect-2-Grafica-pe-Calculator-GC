#version 330 core

layout(location=0) in vec3 in_Position;
 
uniform mat4 model;
uniform mat4 lightSpaceMatrix;


void main(void)
{
	gl_Position = lightSpaceMatrix*model*vec4(in_Position, 1.0);
} 