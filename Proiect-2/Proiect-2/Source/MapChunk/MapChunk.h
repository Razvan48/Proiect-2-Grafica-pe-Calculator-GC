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
	std::vector<glm::vec2> uvs;
	std::vector<GLuint> indices;

	GLuint VAO;
	GLuint VBO;
	GLuint EBO;

	glm::vec2 rotatePoint(float x, float y, float angleDegrees) const;

	static const int MAX_COORDINATE_Y;
	inline int hashCoordinates(int x, int y) const { return x * MapChunk::MAX_COORDINATE_Y + y; }

public:
	MapChunk(int x, int y);
	~MapChunk();

	inline int getX() const { return x; }
	inline int getY() const { return y; }

	void draw();
	void update();

	static int calculateChunkX(float x);
	static int calculateChunkY(float y);

	glm::vec2 getMinXZ() const;
	glm::vec2 getMaxXZ() const;
};
