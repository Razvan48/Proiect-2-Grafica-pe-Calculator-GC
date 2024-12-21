#pragma once

#include <GL/glew.h>

#include <vector>
#include <string>

#include "../Source/Camera/Camera.h"

class Skybox // TODO: mostenire din object
{
public:
	// Constructor
	Skybox();

	// Destructor
	~Skybox();

	void Render(const Camera& camera);

private:
	static const std::vector<std::string> faces;
	GLuint cubemapTextureID;
	GLuint programId;

	GLuint VAO, VBO;

	void LoadCubemap();
	void LoadShadersSkybox();
	void CreateVAO();
};
