#version 330 core

in vec2 fragTexCoord;	
in vec3 fragNormal;

out vec4 out_Color;

uniform sampler2D textureDiffuse;

uniform vec3 directionalLight;

void main(void)
{
	out_Color = texture(textureDiffuse, fragTexCoord);

	// Ambient light
	float ambientStrength = 0.2f;
	vec3 ambientColor = vec3(ambientStrength * out_Color.r, ambientStrength * out_Color.g, ambientStrength * out_Color.b);

	// Diffuse light
	vec3 normalizedFragNormal = normalize(fragNormal);
	vec3 normalizedDirectionalLight = normalize(-directionalLight);

	float diffuseStrength = max(dot(normalizedFragNormal, normalizedDirectionalLight), 0.0f);

	vec3 diffuseColor = vec3(diffuseStrength * out_Color.r, diffuseStrength * out_Color.g, diffuseStrength * out_Color.b);


	out_Color = vec4(ambientColor + diffuseColor, out_Color.a);	
}
