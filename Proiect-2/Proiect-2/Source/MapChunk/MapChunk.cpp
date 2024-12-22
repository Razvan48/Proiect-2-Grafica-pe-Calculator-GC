#include "MapChunk.h"

#include "../PerlinNoise2D/PerlinNoise2D.h"

#include "../Camera/Camera.h"

#include "../Map/Map.h"

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

	// TODO: de modificat in viitor, ca sa avem normale + texturi

	for (int i = 0; i < this->heightMap.size(); ++i)
		for (int j = 0; j < this->heightMap[i].size(); ++j)
			this->vertices.push_back(glm::vec3(this->x * MapChunk::CHUNK_SIZE + i * quadSize, this->heightMap[i][j], this->y * MapChunk::CHUNK_SIZE + j * quadSize));

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

	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(glm::vec3), this->vertices.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), this->indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

MapChunk::~MapChunk()
{

}

void MapChunk::draw()
{
	// TODO: (proiectia e facuta aici si nu e ok) (refactoring, asezat mai frumos aici)

	glUseProgram(Map::get().getProgramId());

	// set value for view matrix
	glm::mat4 projection = glm::perspective(glm::radians(Camera::Get().GetZoom()), 1400.0f / 800.0f, 0.1f, 1000.0f);
	glm::mat4 view = Camera::Get().GetViewMatrix();

	glUniformMatrix4fv(glGetUniformLocation(Map::get().getProgramId(), "projection"), 1, GL_FALSE, &projection[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(Map::get().getProgramId(), "view"), 1, GL_FALSE, &Camera::Get().GetViewMatrix()[0][0]);

	glBindVertexArray(this->VAO);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // TODO: de eliminat, debug, wireframe
	glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // TODO: de eliminat, debug, wireframe
	glBindVertexArray(0);

	glUseProgram(0);
}

void MapChunk::update()
{
	// momentan nimic
}

int MapChunk::calculateChunkX(float x)
{
	return (int)(x / MapChunk::CHUNK_SIZE);
}

int MapChunk::calculateChunkY(float y)
{
	return (int)(y / MapChunk::CHUNK_SIZE);
}

const int MapChunk::CHUNK_SIZE = 16;
const int MapChunk::NUM_QUADS_PER_SIDE = 32;