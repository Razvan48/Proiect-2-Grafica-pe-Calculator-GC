#version 330 core

in vec2 fragTexCoord;	

out vec4 out_Color;

uniform sampler2D textureDiffuse;

void main(void)
{
	// out_Color = texture(textureDiffuse, fragTexCoord);
	out_Color = vec4(1.0, 1.0, 1.0, 1.0); // TODO: delete
}
