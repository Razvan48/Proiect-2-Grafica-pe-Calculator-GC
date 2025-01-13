#version 330 core

in vec2 fragTexCoord;	
in vec3 fragNormal;
in vec4 FragPosLightSpace;

out vec4 out_Color;

uniform sampler2D textureDiffuse;
uniform sampler2D depthMap;

uniform vec3 directionalLight;

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
	
	float shadow = ShadowCalculation(FragPosLightSpace, directionalLight);

	out_Color = vec4(ambientColor +  (1.0 - shadow) * diffuseColor, out_Color.a);	
}
