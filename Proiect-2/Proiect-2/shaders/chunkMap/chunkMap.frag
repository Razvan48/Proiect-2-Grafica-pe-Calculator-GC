#version 330 core

in vec2 texCoord;

in vec3 fragPosition;
in vec3 fragNormal;

uniform vec3 directionalLight;

uniform sampler2D texture0;
uniform sampler2D texture1;

uniform float threshWaterGrass;

out vec4 outFragColor;

float random01(vec2 pos)
{
    return fract(sin(dot(pos, vec2(12.9898, 78.233))) * 43758.5453);
}

const float randomAmplitude = 1.0f;

void main()
{
	if (fragPosition.y > threshWaterGrass + randomAmplitude * random01(fragPosition.xz))
		outFragColor = texture(texture0, texCoord);
	else
		outFragColor = texture(texture1, texCoord);

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
