#version 330 core

layout (location=0) in vec3 inPosition;
layout (location=1) in vec3 inColor;
layout (location=2) in vec2 texCoord;

out vec4 outColorVertex;
out vec4 clipSpace;
out vec2 textureCoords; // for the dudv mapping
out vec3 toCameraVector; // for Fresnel effect

out vec3 fromLightVector; // lighting

uniform mat4 scale;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 cameraPosition;

uniform vec4 lightPosition;

const float tiling = 4.0;

const float texCoordsScaleFactor = 40.0;

void main()
{
	outColorVertex = vec4(inColor, 1.0);

	vec4 worldPosition = model * vec4(inPosition, 1.0);
	clipSpace = projection * view * worldPosition;
	gl_Position = clipSpace;

	// texture coords (for waves)
	vec4 scaledVertexCoords = scale * vec4(inPosition, 1.0);
	textureCoords = vec2((scaledVertexCoords.x + abs(scaledVertexCoords.x)) / texCoordsScaleFactor, (scaledVertexCoords.z + abs(scaledVertexCoords.z)) / texCoordsScaleFactor);


	toCameraVector = cameraPosition - worldPosition.xyz;

	if(lightPosition.w == 0.0) // sursa directionala
	{
		fromLightVector = lightPosition.xyz;
	}
	else  // sursa punctuala
	{
		fromLightVector = worldPosition.xyz - lightPosition.xyz;
	}
}