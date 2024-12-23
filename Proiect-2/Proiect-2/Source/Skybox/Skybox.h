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

	void draw();

private:
	static const std::vector<std::string> faces;
	GLuint cubemapTextureID;
	GLuint programId;

	GLuint VAO, VBO;

	void loadCubemap();
	void loadShadersSkybox();
	void createVAO();
};
