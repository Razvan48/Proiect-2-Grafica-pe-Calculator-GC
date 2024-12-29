#version 330 core

in vec4 outColorVertex;
in vec4 clipSpace;
in vec2 textureCoords; // for the dudv mapping
in vec3 toCameraVector; // for Fresnel effect

in vec3 fromLightVector; // lighting

out vec4 out_Color;

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform sampler2D dudvMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;

uniform float moveFactor;

uniform vec3 lightColor;

const float waveStrength = 0.02;

const float shineDamper = 64.0;
const float reflectivity = 0.3;

const float depthLimit = 1.0;

// for depth mapping
uniform float near;
uniform float far;

void main()
{
	vec2 ndc = (clipSpace.xy/clipSpace.w)/2.0 + 0.5;

	vec2 refractTexCoords = vec2(ndc.x, ndc.y);
	vec2 reflectTexCoords = vec2(ndc.x, 1.0 - ndc.y);

	float depth = texture(depthMap, refractTexCoords).r;
	float floorDistance = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));

	depth = gl_FragCoord.z;
	float waterDistance = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));

	float waterDepth = floorDistance - waterDistance;

	// vec2 distortion1 = (texture(dudvMap, vec2(textureCoords.x + moveFactor, textureCoords.y)).rg * 2.0 - 1.0) * waveStrength;
	// vec2 distortion2 = (texture(dudvMap, vec2(-textureCoords.x + moveFactor, textureCoords.y + moveFactor)).rg * 2.0 - 1.0) * waveStrength;
	// vec2 totalDistortion = distortion1 + distortion2;

	vec2 distortedTexCoords = texture(dudvMap, vec2(textureCoords.x + moveFactor, textureCoords.y)).rg * 0.1;
	distortedTexCoords = textureCoords + vec2(distortedTexCoords.x, distortedTexCoords.y + moveFactor);
	vec2 totalDistortion = (texture(dudvMap, distortedTexCoords).rg * 2.0 - 1.0) * waveStrength * clamp(waterDepth / 20.0, 0.0, 1.0);

	refractTexCoords += totalDistortion;
	refractTexCoords = clamp(refractTexCoords, 0.001, 0.999);

	reflectTexCoords += totalDistortion;
	reflectTexCoords = clamp(reflectTexCoords, 0.001, 0.999);

	vec4 reflectionColor = texture(reflectionTexture, reflectTexCoords);
	vec4 refractionColor = texture(refractionTexture, refractTexCoords);

	vec3 viewVector = normalize(toCameraVector);

	float refractionFactor = dot(viewVector, vec3(0.0, 1.0, 0.0));
	refractionFactor = pow(refractionFactor, 2.0);


	// normal mapping and lighting
	vec4 normalMapColor = texture(normalMap, distortedTexCoords);
	vec3 normal = vec3(normalMapColor.r * 2.0 - 1.0, normalMapColor.b, normalMapColor.g * 2.0 - 1.0);
	normal = normalize(normal);

	vec3 reflectedLight = reflect(normalize(fromLightVector), normal);
	float specular = max(dot(reflectedLight, viewVector), 0.0);
	specular = pow(specular, shineDamper);
	vec3 specularHighlights = lightColor * specular * reflectivity * clamp(waterDepth / 7.0, 0.0, 1.0);

	out_Color = mix(reflectionColor, refractionColor, refractionFactor);
	out_Color = mix(out_Color, outColorVertex, 0.1) + vec4(specularHighlights, 0.0);
	out_Color.a = clamp(waterDepth / depthLimit, 0.0, 1.0);
}