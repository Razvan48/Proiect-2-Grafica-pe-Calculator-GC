#include "MapChunk.h"

#include "../PerlinNoise2D/PerlinNoise2D.h"

#include "../Camera/Camera.h"

#include "../Map/Map.h"

#include "../TextureManager/TextureManager.h"

#include "../RandomGenerator/RandomGenerator.h"

MapChunk::MapChunk(int x, int y)
	: x(x), y(y), openGLSetupDone(false)
{
	float quadSize = (float)MapChunk::CHUNK_SIZE / MapChunk::NUM_QUADS_PER_SIDE;

	float minimumHeight = MapChunk::INF_HEIGHT;
	this->heightMap.resize(MapChunk::NUM_QUADS_PER_SIDE + 1);
	for (int i = 0; i < this->heightMap.size(); ++i)
	{
		this->heightMap[i].resize(MapChunk::NUM_QUADS_PER_SIDE + 1);

		for (int j = 0; j < this->heightMap[i].size(); ++j)
		{
			this->heightMap[i][j] = PerlinNoise2D::get().perlinNoise2D(this->x * MapChunk::CHUNK_SIZE + j * quadSize, (this->y + 1) * MapChunk::CHUNK_SIZE - i * quadSize);
			minimumHeight = glm::min(minimumHeight, this->heightMap[i][j]);
		}
	}
	minimumHeight -= MapChunk::DELTA_CULLING_SHADOW_MAPPING;

	// TODO: de modificat in viitor, ca sa avem normale (inclusiv la cele 4 for-uri trebuie normale) + schimbare in vertex attributes + in shader
	RandomGenerator randomGenerator(this->hashCoordinates(this->x, this->y));
	float angleDegrees = randomGenerator.randomUniformDouble(0, 360.0);

	for (int i = 0; i < this->heightMap.size(); ++i)
	{
		for (int j = 0; j < this->heightMap[i].size(); ++j)
		{
			this->vertices.push_back(glm::vec3(this->x * MapChunk::CHUNK_SIZE + j * quadSize, this->heightMap[i][j], (this->y + 1) * MapChunk::CHUNK_SIZE - i * quadSize));

			float uvX = (float)j / MapChunk::NUM_QUADS_PER_SIDE;
			float uvY = 1.0f - (float)i / MapChunk::NUM_QUADS_PER_SIDE;

			this->uvs.emplace_back(this->rotatePoint(uvX, uvY, angleDegrees));
		}
	}

	// trebuie normale si aici
	// ordine: sus, jos, stanga, dreapta
	for (int j = 0; j < MapChunk::NUM_QUADS_PER_SIDE + 1; ++j)
	{
		this->vertices.push_back(glm::vec3(this->x * MapChunk::CHUNK_SIZE + j * quadSize, minimumHeight, 1.0f * (this->y + 1) * MapChunk::CHUNK_SIZE));

		float uvX = (float)j / MapChunk::NUM_QUADS_PER_SIDE;	
		float uvY = 1.0f;

		this->uvs.emplace_back(this->rotatePoint(uvX, uvY, angleDegrees));
	}
	for (int j = 0; j < MapChunk::NUM_QUADS_PER_SIDE + 1; ++j)
	{
		this->vertices.push_back(glm::vec3(this->x * MapChunk::CHUNK_SIZE + j * quadSize, minimumHeight, 1.0f * this->y * MapChunk::CHUNK_SIZE));

		float uvX = (float)j / MapChunk::NUM_QUADS_PER_SIDE;
		float uvY = 0.0f;

		this->uvs.emplace_back(this->rotatePoint(uvX, uvY, angleDegrees));
	}
	for (int i = 0; i < MapChunk::NUM_QUADS_PER_SIDE + 1; ++i)
	{
		this->vertices.push_back(glm::vec3(1.0f * this->x * MapChunk::CHUNK_SIZE, minimumHeight, (this->y + 1) * MapChunk::CHUNK_SIZE - i * quadSize));

		float uvX = 0.0f;
		float uvY = 1.0f - (float)i / MapChunk::NUM_QUADS_PER_SIDE;

		this->uvs.emplace_back(this->rotatePoint(uvX, uvY, angleDegrees));
	}
	for (int i = 0; i < MapChunk::NUM_QUADS_PER_SIDE + 1; ++i)
	{
		this->vertices.push_back(glm::vec3(1.0f * (this->x + 1) * MapChunk::CHUNK_SIZE, minimumHeight, (this->y + 1) * MapChunk::CHUNK_SIZE - i * quadSize));

		float uvX = 1.0f;
		float uvY = 1.0f - (float)i / MapChunk::NUM_QUADS_PER_SIDE;

		this->uvs.emplace_back(this->rotatePoint(uvX, uvY, angleDegrees));
	}

	// Indices
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
	// sus
	for (int j = 1; j < MapChunk::NUM_QUADS_PER_SIDE + 1; ++j)
	{
		int i = 0;

		this->indices.push_back((MapChunk::NUM_QUADS_PER_SIDE + 1) * (MapChunk::NUM_QUADS_PER_SIDE + 1) + j - 1);
		this->indices.push_back(i * (MapChunk::NUM_QUADS_PER_SIDE + 1) + j - 1);
		this->indices.push_back(i * (MapChunk::NUM_QUADS_PER_SIDE + 1) + j);

		this->indices.push_back((MapChunk::NUM_QUADS_PER_SIDE + 1) * (MapChunk::NUM_QUADS_PER_SIDE + 1) + j - 1);
		this->indices.push_back(i * (MapChunk::NUM_QUADS_PER_SIDE + 1) + j);
		this->indices.push_back((MapChunk::NUM_QUADS_PER_SIDE + 1) * (MapChunk::NUM_QUADS_PER_SIDE + 1) + j);
	}
	// jos
	for (int j = 1; j < MapChunk::NUM_QUADS_PER_SIDE + 1; ++j)
	{
		int i = MapChunk::NUM_QUADS_PER_SIDE;

		this->indices.push_back((MapChunk::NUM_QUADS_PER_SIDE + 1) * (MapChunk::NUM_QUADS_PER_SIDE + 1) + (MapChunk::NUM_QUADS_PER_SIDE + 1) + j);
		this->indices.push_back(i * (MapChunk::NUM_QUADS_PER_SIDE + 1) + j);
		this->indices.push_back(i * (MapChunk::NUM_QUADS_PER_SIDE + 1) + j - 1);

		this->indices.push_back((MapChunk::NUM_QUADS_PER_SIDE + 1) * (MapChunk::NUM_QUADS_PER_SIDE + 1) + (MapChunk::NUM_QUADS_PER_SIDE + 1) + j);
		this->indices.push_back(i * (MapChunk::NUM_QUADS_PER_SIDE + 1) + j - 1);
		this->indices.push_back((MapChunk::NUM_QUADS_PER_SIDE + 1) * (MapChunk::NUM_QUADS_PER_SIDE + 1) + (MapChunk::NUM_QUADS_PER_SIDE + 1) + j - 1);
	}
	// stanga
	for (int i = 1; i < MapChunk::NUM_QUADS_PER_SIDE + 1; ++i)
	{
		int j = 0;

		this->indices.push_back((MapChunk::NUM_QUADS_PER_SIDE + 1) * (MapChunk::NUM_QUADS_PER_SIDE + 1) + 2 * (MapChunk::NUM_QUADS_PER_SIDE + 1) + i);
		this->indices.push_back(i * (MapChunk::NUM_QUADS_PER_SIDE + 1) + j);
		this->indices.push_back((i - 1) * (MapChunk::NUM_QUADS_PER_SIDE + 1) + j);

		this->indices.push_back((MapChunk::NUM_QUADS_PER_SIDE + 1) * (MapChunk::NUM_QUADS_PER_SIDE + 1) + 2 * (MapChunk::NUM_QUADS_PER_SIDE + 1) + i);
		this->indices.push_back((i - 1) * (MapChunk::NUM_QUADS_PER_SIDE + 1) + j);
		this->indices.push_back((MapChunk::NUM_QUADS_PER_SIDE + 1) * (MapChunk::NUM_QUADS_PER_SIDE + 1) + 2 * (MapChunk::NUM_QUADS_PER_SIDE + 1) + i - 1);
	}
	// dreapta
	for (int i = 1; i < MapChunk::NUM_QUADS_PER_SIDE + 1; ++i)
	{
		int j = MapChunk::NUM_QUADS_PER_SIDE;

		this->indices.push_back((MapChunk::NUM_QUADS_PER_SIDE + 1) * (MapChunk::NUM_QUADS_PER_SIDE + 1) + 3 * (MapChunk::NUM_QUADS_PER_SIDE + 1) + i - 1);
		this->indices.push_back((i - 1) * (MapChunk::NUM_QUADS_PER_SIDE + 1) + j);
		this->indices.push_back(i * (MapChunk::NUM_QUADS_PER_SIDE + 1) + j);

		this->indices.push_back((MapChunk::NUM_QUADS_PER_SIDE + 1) * (MapChunk::NUM_QUADS_PER_SIDE + 1) + 3 * (MapChunk::NUM_QUADS_PER_SIDE + 1) + i - 1);
		this->indices.push_back(i * (MapChunk::NUM_QUADS_PER_SIDE + 1) + j);
		this->indices.push_back((MapChunk::NUM_QUADS_PER_SIDE + 1) * (MapChunk::NUM_QUADS_PER_SIDE + 1) + 3 * (MapChunk::NUM_QUADS_PER_SIDE + 1) + i);
	}

	// fata de jos
	this->indices.push_back((MapChunk::NUM_QUADS_PER_SIDE + 1) * (MapChunk::NUM_QUADS_PER_SIDE + 1) + (MapChunk::NUM_QUADS_PER_SIDE + 1));
	this->indices.push_back((MapChunk::NUM_QUADS_PER_SIDE + 1) * (MapChunk::NUM_QUADS_PER_SIDE + 1));
	this->indices.push_back((MapChunk::NUM_QUADS_PER_SIDE + 1) * (MapChunk::NUM_QUADS_PER_SIDE + 1) + 3 * (MapChunk::NUM_QUADS_PER_SIDE + 1));

	this->indices.push_back((MapChunk::NUM_QUADS_PER_SIDE + 1) * (MapChunk::NUM_QUADS_PER_SIDE + 1) + (MapChunk::NUM_QUADS_PER_SIDE + 1));
	this->indices.push_back((MapChunk::NUM_QUADS_PER_SIDE + 1) * (MapChunk::NUM_QUADS_PER_SIDE + 1) + 3 * (MapChunk::NUM_QUADS_PER_SIDE + 1));
	this->indices.push_back((MapChunk::NUM_QUADS_PER_SIDE + 1) * (MapChunk::NUM_QUADS_PER_SIDE + 1) + 4 * (MapChunk::NUM_QUADS_PER_SIDE + 1) - 1);
}

void MapChunk::setupOpenGL()
{
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

	this->openGLSetupDone = true;
}

MapChunk::MapChunk(MapChunk&& other) noexcept
	: x(other.x), y(other.y)
	, heightMap(std::move(other.heightMap))
	, vertices(std::move(other.vertices))
	, uvs(std::move(other.uvs))
	, indices(std::move(other.indices))
	, VAO(other.VAO), VBO(other.VBO), EBO(other.EBO)
	, openGLSetupDone(other.openGLSetupDone)
{
	other.VAO = 0;
	other.VBO = 0;
	other.EBO = 0;
}

MapChunk& MapChunk::operator= (MapChunk&& other) noexcept
{
	if (this == &other)
		return *this;

	this->x = other.x;
	this->y = other.y;
	this->heightMap = std::move(other.heightMap);
	this->vertices = std::move(other.vertices);
	this->uvs = std::move(other.uvs);
	this->indices = std::move(other.indices);
	this->VAO = other.VAO;
	this->VBO = other.VBO;
	this->EBO = other.EBO;
	this->openGLSetupDone = other.openGLSetupDone;

	other.VAO = 0;
	other.VBO = 0;
	other.EBO = 0;

	return *this;
}

MapChunk::~MapChunk()
{
	if (this->VAO)
		glDeleteVertexArrays(1, &this->VAO);
	if (this->VBO)
		glDeleteBuffers(1, &this->VBO);
	if (this->EBO)
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

	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // test
	// cull front
	glFrontFace(GL_CW);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
	glDisable(GL_CULL_FACE);
	// glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // test

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

const int MapChunk::CHUNK_SIZE = 48;
const int MapChunk::NUM_QUADS_PER_SIDE = 16;
const int MapChunk::MAX_COORDINATE_Y = 666013;

const float MapChunk::DELTA_CULLING_SHADOW_MAPPING = 1.0f;
const float MapChunk::INF_HEIGHT = 1000000.0f;