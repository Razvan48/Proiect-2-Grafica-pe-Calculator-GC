#pragma once

#include <GL/glew.h>

#include <glm/glm.hpp>

#include <vector>
#include <string>

#include "../MapChunk/MapChunk.h"

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
	glm::vec3 color;

	bool operator == (const Vertex& other) const { return position == other.position && normal == other.normal && uv == other.uv && color == other.color; }
};

class Model
{
public:
	// Contructor
	Model(const std::string& objFilePath, const std::string& textureName);

	// Destructor
	~Model();

	void draw(const GLuint& programId, const glm::mat4& model, MapChunk* mapChunk = nullptr, bool isDrawingDepthMap = false);

private:
	const std::string textureName;

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	GLuint VAO, VBO, EBO;

	void createVAO();
};
