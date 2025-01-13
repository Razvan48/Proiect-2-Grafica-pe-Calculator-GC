#version 330 core

in vec2 texCoord;

in vec3 fragPosition;
in vec3 fragNormal;
in vec4 FragPosLightSpace;

uniform vec3 directionalLight;

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D depthMap;

uniform float threshWaterGrass;

out vec4 outFragColor;

float random01(vec2 pos)
{
    return fract(sin(dot(pos, vec2(12.9898, 78.233))) * 43758.5453);
}

const float randomAmplitude = 1.0f;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if(projCoords.z > 1.0) return 0.0;

    float closestDepth = texture(depthMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;
    float bias = max(0.02 * (1.0 - dot(fragNormal, lightDir)), 0.005); 
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

    return shadow;
}


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

	float shadow = ShadowCalculation(FragPosLightSpace, directionalLight);

	outFragColor = vec4(ambientColor +  (1.0 - shadow) * diffuseColor, outFragColor.a);	
}
