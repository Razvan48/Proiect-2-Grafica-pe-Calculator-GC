#pragma once

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <memory>

#include "../Grass/Grass.h"

class MapChunk
{
private:
	int x;
	int y;

	static const int NUM_QUADS_PER_SIDE;

	std::vector<std::vector<float>> heightMap;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
	std::vector<GLuint> indices;

	GLuint VAO;
	GLuint VBO;
	GLuint EBO;

	std::vector<Blade> grassBlades;
	std::unique_ptr<Grass> grass;

	bool openGLSetupDone;

	glm::vec2 rotatePoint(float x, float y, float angleDegrees) const;

	static const int MAX_COORDINATE_Y;
	inline int hashCoordinates(int x, int y) const { return x * MapChunk::MAX_COORDINATE_Y + y; }

	static const float DELTA_CULLING_SHADOW_MAPPING;
	static const float INF_HEIGHT;

	static glm::vec3 directionalLight;
	static const float DAY_NIGHT_CYCLE_SPEED;

	void generateGrass();
	glm::vec3 generateRandomPointInTriangle(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C) const;
	bool isCameraInChunk() const;

public:
	MapChunk(int x, int y);
	MapChunk(const MapChunk& other) = delete;
	MapChunk& operator= (const MapChunk& other) = delete;
	MapChunk(MapChunk&& other) noexcept;
	MapChunk& operator= (MapChunk&& other) noexcept;
	~MapChunk();
	void setupOpenGL();

	static const int CHUNK_SIZE;

	inline int getX() const { return x; }
	inline int getY() const { return y; }

	void draw();
	void update();
	static void commonUpdate();

	static int calculateChunkX(float x);
	static int calculateChunkY(float y);

	inline bool getOpenGLSetupDone() const { return this->openGLSetupDone; }

	static glm::vec3 getDirectionalLight() { return directionalLight; }
};
