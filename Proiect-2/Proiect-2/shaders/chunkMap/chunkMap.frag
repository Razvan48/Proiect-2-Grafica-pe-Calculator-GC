#version 330 core

in vec2 texCoord;

uniform sampler2D texture0;

out vec4 outFragColor;

void main()
{    
	outFragColor = texture(texture0, texCoord);
}
