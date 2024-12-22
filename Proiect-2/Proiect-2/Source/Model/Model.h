#pragma once

#include <GL/glew.h>

#include <glm/glm.hpp>

#include <vector>
#include <string>

#include "../Source/Camera/Camera.h"

class Model
{
public:
	// Contructor
	Model(const std::string& objFilePath);

	// Destructor
	~Model();

	void Render(const Camera& camera, const GLuint& programId);

private:
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;

	GLuint VAO, VBO;

	void CreateVAO();
};