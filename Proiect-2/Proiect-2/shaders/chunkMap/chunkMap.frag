#version 330 core

in vec2 texCoord;

in vec3 fragPosition;
in vec3 fragNormal;

uniform vec3 directionalLight;

uniform sampler2D texture0;

out vec4 outFragColor;

void main()
{
	outFragColor = texture(texture0, texCoord);

	// Ambient light
	float ambientStrength = 0.2f;
	vec3 ambientColor = vec3(ambientStrength * outFragColor.r, ambientStrength * outFragColor.g, ambientStrength * outFragColor.b);

	// Diffuse light
	vec3 normalizedFragNormal = normalize(fragNormal);
	vec3 normalizedDirectionalLight = normalize(-directionalLight);

	float diffuseStrength = max(dot(normalizedFragNormal, normalizedDirectionalLight), 0.0f);

	vec3 diffuseColor = vec3(diffuseStrength * outFragColor.r, diffuseStrength * outFragColor.g, diffuseStrength * outFragColor.b);


	outFragColor = vec4(ambientColor + diffuseColor, outFragColor.a);
}
