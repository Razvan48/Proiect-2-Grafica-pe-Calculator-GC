#pragma once

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

class MapChunk
{
private:
	int x;
	int y;

	static const int CHUNK_SIZE;
	static const int NUM_QUADS_PER_SIDE;

	std::vector<std::vector<float>> heightMap;
	std::vector<glm::vec3> vertices;
	std::vector<GLuint> indices;

	GLuint VAO;
	GLuint VBO;
	GLuint EBO;

public:
	MapChunk(int x, int y);
	~MapChunk();

	inline int getX() const { return x; }
	inline int getY() const { return y; }

	void draw();
	void update();

	static int calculateChunkX(float x);
	static int calculateChunkY(float y);
};
