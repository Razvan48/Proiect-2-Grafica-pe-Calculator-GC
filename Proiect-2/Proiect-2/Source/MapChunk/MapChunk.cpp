#include "MapChunk.h"

#include "../PerlinNoise2D/PerlinNoise2D.h"

#include "../Camera/Camera.h"

#include "../Map/Map.h"

#include "../TextureManager/TextureManager.h"

#include "../RandomGenerator/RandomGenerator.h"

MapChunk::MapChunk(int x, int y)
	: x(x), y(y)
{
	float quadSize = (float)MapChunk::CHUNK_SIZE / MapChunk::NUM_QUADS_PER_SIDE;

	this->heightMap.resize(MapChunk::NUM_QUADS_PER_SIDE + 1);
	for (int i = 0; i < this->heightMap.size(); ++i)
	{
		this->heightMap[i].resize(MapChunk::NUM_QUADS_PER_SIDE + 1);

		for (int j = 0; j < this->heightMap[i].size(); ++j)
			this->heightMap[i][j] = PerlinNoise2D::get().perlinNoise2D(this->x * MapChunk::CHUNK_SIZE + i * quadSize, this->y * MapChunk::CHUNK_SIZE + j * quadSize);
	}

	// TODO: de modificat in viitor, ca sa avem normale
	RandomGenerator::get().setSeed(this->hashCoordinates(this->x, this->y));
	float angleDegrees = RandomGenerator::get().randomUniformDouble(0, 360.0);

	for (int i = 0; i < this->heightMap.size(); ++i)
	{
		for (int j = 0; j < this->heightMap[i].size(); ++j)
		{
			this->vertices.push_back(glm::vec3(this->x * MapChunk::CHUNK_SIZE + i * quadSize, this->heightMap[i][j], this->y * MapChunk::CHUNK_SIZE + j * quadSize));

			float uvX = (float)i / MapChunk::NUM_QUADS_PER_SIDE;
			float uvY = (float)j / MapChunk::NUM_QUADS_PER_SIDE;

			this->uvs.emplace_back(this->rotatePoint(uvX, uvY, angleDegrees));
		}
	}

	for (int i = 1; i < this->heightMap.size(); ++i)
	{
		for (int j = 1; j < this->heightMap[i].size(); ++j)
		{
			this->indices.push_back(i * this->heightMap[i].size() + j);
			this->indices.push_back((i - 1) * this->heightMap[i - 1].size() + j);
			this->indices.push_back((i - 1) * this->heightMap[i - 1].size() + j - 1);

			this->indices.push_back(i * this->heightMap[i].size() + j);
			this->indices.push_back((i - 1) * this->heightMap[i - 1].size() + j - 1);
			this->indices.push_back(i * this->heightMap[i].size() + j - 1);
		}
	}


	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glGenBuffers(1, &this->EBO);

	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);

	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(glm::vec3) + this->uvs.size() * sizeof(glm::vec2), nullptr, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, this->vertices.size() * sizeof(glm::vec3), this->vertices.data());
	glBufferSubData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(glm::vec3), this->uvs.size() * sizeof(glm::vec2), this->uvs.data());
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), this->indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)(this->vertices.size() * sizeof(glm::vec3)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

MapChunk::~MapChunk()
{
	glDeleteVertexArrays(1, &this->VAO);
	glDeleteBuffers(1, &this->VBO);
	glDeleteBuffers(1, &this->EBO);
}

void MapChunk::draw()
{
	glUseProgram(Map::get().getProgramId());

	// set value for view matrix
	glUniformMatrix4fv(glGetUniformLocation(Map::get().getProgramId(), "projection"), 1, GL_FALSE, &Camera::get().getProjectionMatrix()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(Map::get().getProgramId(), "view"), 1, GL_FALSE, &Camera::get().getViewMatrix()[0][0]);

	// set texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureManager::get().getTexture("grass1"));
	glUniform1i(glGetUniformLocation(Map::get().getProgramId(), "texture0"), 0);

	// draw
	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glUseProgram(0);
}

void MapChunk::update()
{
	// momentan nimic
}

int MapChunk::calculateChunkX(float x)
{
	if (x < 0.0f)
		return 0;
	return (int)(x / MapChunk::CHUNK_SIZE);
}

int MapChunk::calculateChunkY(float y)
{
	if (y < 0.0f)
		return 0;
	return (int)(y / MapChunk::CHUNK_SIZE);
}

glm::vec2 MapChunk::rotatePoint(float x, float y, float angleDegrees) const
{
	float rotatedX = x * glm::cos(glm::radians(angleDegrees)) - y * glm::sin(glm::radians(angleDegrees));
	float rotatedY = x * glm::sin(glm::radians(angleDegrees)) + y * glm::cos(glm::radians(angleDegrees));

	return glm::vec2(rotatedX, rotatedY);
}

glm::vec2 MapChunk::getMinXZ() const
{
	glm::vec2 result = glm::vec2(0.0f, 0.0f);
	if (!vertices.empty())
	{
		result.x = vertices[0].x;
		result.y = vertices[0].z;
	}
	for (size_t i = 0;i < vertices.size(); ++i)
	{
		result.x = std::min(result.x, vertices[i].x);
		result.y = std::min(result.y, vertices[i].z);
	}
	return result;
}

glm::vec2 MapChunk::getMaxXZ() const
{
	glm::vec2 result = glm::vec2(0.0f, 0.0f);
	if (!vertices.empty())
	{
		result.x = vertices[0].x;
		result.y = vertices[0].z;
	}
	for (size_t i = 0;i < vertices.size(); ++i)
	{
		result.x = std::max(result.x, vertices[i].x);
		result.y = std::max(result.y, vertices[i].z);
	}
	return result;
}

const int MapChunk::CHUNK_SIZE = 16;
const int MapChunk::NUM_QUADS_PER_SIDE = 32;
const int MapChunk::MAX_COORDINATE_Y = 666013;