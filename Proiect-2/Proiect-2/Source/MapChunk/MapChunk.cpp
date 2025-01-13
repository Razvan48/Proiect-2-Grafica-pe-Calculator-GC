#include "MapChunk.h"

#include <glm/gtx/transform.hpp>
#include <iostream>

#include "../PerlinNoise2D/PerlinNoise2D.h"
#include "../Camera/Camera.h"
#include "../Map/Map.h"
#include "../TextureManager/TextureManager.h"
#include "../RandomGenerator/RandomGenerator.h"
#include "../GlobalClock/GlobalClock.h"
#include "../Water/Water.h"
#include "../WindowManager/WindowManager.h"
#include "loadShaders.h"

MapChunk::MapChunk(int x, int y)
	: x(x), y(y), openGLSetupDone(false)
	, grass(nullptr)
{
	depthMap.setShadowHeight(WindowManager::get().getHeight() * 2);
	depthMap.setShadowWidth(WindowManager::get().getWidth() * 2);
	depthMap.CreateFBO();
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

	RandomGenerator randomGenerator(this->hashCoordinates(this->x, this->y));
	float angleDegrees = randomGenerator.randomUniformDouble(0, 360.0);

	for (int i = 0; i < this->heightMap.size(); ++i)
	{
		for (int j = 0; j < this->heightMap[i].size(); ++j)
		{
			this->vertices.push_back(glm::vec3(this->x * MapChunk::CHUNK_SIZE + j * quadSize, this->heightMap[i][j], (this->y + 1) * MapChunk::CHUNK_SIZE - i * quadSize));

			// calcul pentru normale // TODO: de vazut daca aceste calcule sunt corecte
			float leftHeight;
			if (j == 0)
			{
				leftHeight = PerlinNoise2D::get().perlinNoise2D(this->x * MapChunk::CHUNK_SIZE - quadSize, (this->y + 1) * MapChunk::CHUNK_SIZE - i * quadSize);
			}
			else
				leftHeight = this->heightMap[i][j - 1];

			float rightHeight;
			if (j == MapChunk::NUM_QUADS_PER_SIDE)
			{
				rightHeight = PerlinNoise2D::get().perlinNoise2D((this->x + 1) * MapChunk::CHUNK_SIZE + quadSize, (this->y + 1) * MapChunk::CHUNK_SIZE - i * quadSize);
			}
			else
				rightHeight = this->heightMap[i][j + 1];

			float upHeight;
			if (i == 0)
			{
				upHeight = PerlinNoise2D::get().perlinNoise2D(this->x * MapChunk::CHUNK_SIZE + j * quadSize, (this->y + 1) * MapChunk::CHUNK_SIZE + quadSize);
			}
			else
				upHeight = this->heightMap[i - 1][j];

			float downHeight;
			if (i == MapChunk::NUM_QUADS_PER_SIDE)
			{
				downHeight = PerlinNoise2D::get().perlinNoise2D(this->x * MapChunk::CHUNK_SIZE + j * quadSize, this->y * MapChunk::CHUNK_SIZE - quadSize);
			}
			else
				downHeight = this->heightMap[i + 1][j];

			float delta = 2 * quadSize;
			float derivativeLeftRight = (rightHeight - leftHeight) / delta;
			float derivativeUpDown = (upHeight - downHeight) / delta;

			glm::vec3 tangentVectorX = glm::vec3(1.0f, derivativeLeftRight, 0.0f);
			glm::vec3 tangentVectorY = glm::vec3(0.0f, derivativeUpDown, 1.0f);

			glm::vec3 normal = glm::normalize(glm::cross(tangentVectorX, tangentVectorY));

			// mai trebuie? // TODO:
			if (normal.y < 0.0f)
				normal = -normal;
			//

			this->normals.push_back(normal);
			//

			float uvX = (float)j / MapChunk::NUM_QUADS_PER_SIDE;
			float uvY = 1.0f - (float)i / MapChunk::NUM_QUADS_PER_SIDE;

			this->uvs.emplace_back(this->rotatePoint(uvX, uvY, angleDegrees));
		}
	}

	// ordine: sus, jos, stanga, dreapta
	for (int j = 0; j < MapChunk::NUM_QUADS_PER_SIDE + 1; ++j)
	{
		this->vertices.push_back(glm::vec3(this->x * MapChunk::CHUNK_SIZE + j * quadSize, minimumHeight, 1.0f * (this->y + 1) * MapChunk::CHUNK_SIZE));

		this->normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f)); // sunt valori foarte aproximative, dar fetele acestea nu ar trebui sa fie vazute

		float uvX = (float)j / MapChunk::NUM_QUADS_PER_SIDE;	
		float uvY = 1.0f;

		this->uvs.emplace_back(this->rotatePoint(uvX, uvY, angleDegrees));
	}
	for (int j = 0; j < MapChunk::NUM_QUADS_PER_SIDE + 1; ++j)
	{
		this->vertices.push_back(glm::vec3(this->x * MapChunk::CHUNK_SIZE + j * quadSize, minimumHeight, 1.0f * this->y * MapChunk::CHUNK_SIZE));

		this->normals.push_back(glm::vec3(0.0f, 0.0f, -1.0f)); // sunt valori foarte aproximative, dar fetele acestea nu ar trebui sa fie vazute

		float uvX = (float)j / MapChunk::NUM_QUADS_PER_SIDE;
		float uvY = 0.0f;

		this->uvs.emplace_back(this->rotatePoint(uvX, uvY, angleDegrees));
	}
	for (int i = 0; i < MapChunk::NUM_QUADS_PER_SIDE + 1; ++i)
	{
		this->vertices.push_back(glm::vec3(1.0f * this->x * MapChunk::CHUNK_SIZE, minimumHeight, (this->y + 1) * MapChunk::CHUNK_SIZE - i * quadSize));

		this->normals.push_back(glm::vec3(-1.0f, 0.0f, 0.0f)); // sunt valori foarte aproximative, dar fetele acestea nu ar trebui sa fie vazute

		float uvX = 0.0f;
		float uvY = 1.0f - (float)i / MapChunk::NUM_QUADS_PER_SIDE;

		this->uvs.emplace_back(this->rotatePoint(uvX, uvY, angleDegrees));
	}
	for (int i = 0; i < MapChunk::NUM_QUADS_PER_SIDE + 1; ++i)
	{
		this->vertices.push_back(glm::vec3(1.0f * (this->x + 1) * MapChunk::CHUNK_SIZE, minimumHeight, (this->y + 1) * MapChunk::CHUNK_SIZE - i * quadSize));

		this->normals.push_back(glm::vec3(1.0f, 0.0f, 0.0f)); // sunt valori foarte aproximative, dar fetele acestea nu ar trebui sa fie vazute

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

	// grass blades
	for (int i = 1; i < this->heightMap.size(); ++i)
	{
		for (int j = 1; j < this->heightMap[i].size(); ++j)
		{
			GLuint index0 = i * this->heightMap[i].size() + j;
			GLuint index1 = (i - 1) * this->heightMap[i - 1].size() + j;
			GLuint index2 = (i - 1) * this->heightMap[i - 1].size() + j - 1;

			GLuint index3 = i * this->heightMap[i].size() + j;
			GLuint index4 = (i - 1) * this->heightMap[i - 1].size() + j - 1;
			GLuint index5 = i * this->heightMap[i].size() + j - 1;

			for (int k = 0; k < GENERATE_GRASS_COUNTER; ++k)
			{
				const glm::vec3 pos1 = generateRandomPointInTriangle(vertices[index0], vertices[index1], vertices[index2]);
				const glm::vec3 pos2 = generateRandomPointInTriangle(vertices[index3], vertices[index4], vertices[index5]);

				if (pos1.y > Water::getHeight() + Grass::getThresholdWaterGrass())
				{
					grassBlades.push_back(Grass::generateBlade(pos1));
				}

				if (pos2.y > Water::getHeight() + Grass::getThresholdWaterGrass())
				{
					grassBlades.push_back(Grass::generateBlade(pos2));
				}
			}
		}
	}

	// trees
	for (int i = 0; i < this->heightMap.size(); ++i)
	{
		for (int j = 0; j < this->heightMap[i].size(); ++j)
		{
			if (this->heightMap[i][j] > Water::getHeight() + Grass::getThresholdWaterGrass() + MapChunk::GRASS_TREE_THRESH && randomGenerator.randomUniformDouble(0.0, 1.0) < MapChunk::TREE_PROBABILITY)
			{
				glm::vec3 treePos = glm::vec3(this->x * MapChunk::CHUNK_SIZE + j * quadSize, this->heightMap[i][j] + MapChunk::TREE_BASE_OFFSET, (this->y + 1) * MapChunk::CHUNK_SIZE - i * quadSize);
				float treeAngle = randomGenerator.randomUniformDouble(0.0, 360.0);
				glm::vec3 treeScale = glm::vec3(randomGenerator.randomUniformDouble(MapChunk::TREE_MIN_SCALE, MapChunk::TREE_MAX_SCALE));

				this->trees.push_back(std::make_pair(treePos, std::make_pair(treeAngle, treeScale)));
			}
		}
	}

	// Boats
	for (int i = 0; i < this->heightMap.size(); ++i)
	{
		for (int j = 0; j < this->heightMap[i].size(); ++j)
		{
			if (this->heightMap[i][j] < Water::getHeight() + MapChunk::WATER_BOAT_THRESH && randomGenerator.randomUniformDouble(0.0, 1.0) < MapChunk::BOAT_PROBABILITY)
			{
				glm::vec3 boatPos = glm::vec3(this->x * MapChunk::CHUNK_SIZE + j * quadSize, Water::getHeight() + MapChunk::BOAT_BASE_OFFSET, (this->y + 1) * MapChunk::CHUNK_SIZE - i * quadSize);
				float boatAngle = randomGenerator.randomUniformDouble(0.0, 360.0);
				glm::vec3 boatScale = glm::vec3(randomGenerator.randomUniformDouble(MapChunk::BOAT_MIN_SCALE, MapChunk::BOAT_MAX_SCALE));

				this->boats.push_back(std::make_pair(boatPos, std::make_pair(boatAngle, boatScale)));
			}
		}
	}
}

void MapChunk::setupOpenGL()
{
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glGenBuffers(1, &this->EBO);

	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);

	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(glm::vec3) + this->normals.size() * sizeof(glm::vec3) + this->uvs.size() * sizeof(glm::vec2), nullptr, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, this->vertices.size() * sizeof(glm::vec3), this->vertices.data());
	glBufferSubData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(glm::vec3), this->normals.size() * sizeof(glm::vec3), this->normals.data());
	glBufferSubData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(glm::vec3) + this->normals.size() * sizeof(glm::vec3), this->uvs.size() * sizeof(glm::vec2), this->uvs.data());
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), this->indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)(this->vertices.size() * sizeof(glm::vec3)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)(this->vertices.size() * sizeof(glm::vec3) + this->normals.size() * sizeof(glm::vec3)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	this->openGLSetupDone = true;
}

void MapChunk::generateGrass()
{
	grass = std::make_unique<Grass>();
	grass->createVAO(grassBlades);
}

glm::vec3 MapChunk::generateRandomPointInTriangle(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C) const
{
	float r1 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	float r2 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

	if (r1 + r2 > 1)
	{
		r1 = 1 - r1;
		r2 = 1 - r2;
	}

	float lambda1 = r1;
	float lambda2 = r2;
	float lambda3 = 1.0f - lambda1 - lambda2;

	return lambda1 * A + lambda2 * B + lambda3 * C;
}

bool MapChunk::isCameraInChunk()
{
	int cameraChunkX = MapChunk::calculateChunkX(Camera::get().getPosition().x);
	int cameraChunkY = MapChunk::calculateChunkY(Camera::get().getPosition().z);

	return cameraChunkX == x && cameraChunkY == y;

	//return std::abs(cameraChunkX - x) <= 1 && std::abs(cameraChunkY - y) <= 1;
}

MapChunk::MapChunk(MapChunk&& other) noexcept
	: x(other.x), y(other.y)
	, heightMap(std::move(other.heightMap))
	, vertices(std::move(other.vertices))
	, normals(std::move(other.normals))
	, uvs(std::move(other.uvs))
	, indices(std::move(other.indices))
	, VAO(other.VAO), VBO(other.VBO), EBO(other.EBO)
	, grassBlades(std::move(other.grassBlades))
	, grass(std::move(other.grass))
	, openGLSetupDone(other.openGLSetupDone)
	, trees(std::move(other.trees))
	, boats(std::move(other.boats))
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
	this->normals = std::move(other.normals);
	this->uvs = std::move(other.uvs);
	this->indices = std::move(other.indices);
	this->VAO = other.VAO;
	this->VBO = other.VBO;
	this->EBO = other.EBO;
	this->grassBlades = std::move(other.grassBlades);
	this->grass = std::move(other.grass);
	this->openGLSetupDone = other.openGLSetupDone;
	this->trees = std::move(other.trees);
	this->boats = std::move(other.boats);

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

	if (grass)
	{
		grass.reset();
	}
}


void MapChunk::outPutShadowMap() {
	// Set viewport to match the window size or shadow map dimensions
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Define the quad vertices, mapping (-1, 1) to screen space
	float quadVertices[] = {
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	};

	GLuint quadVAO, quadVBO;

	// Generate and bind the VAO/VBO for the quad
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	// Load and use the shaders to render depth map

	GLuint ProgramId = LoadShaders("shaders/depthMap/outPutShadowMap.vert", "shaders/depthMap/outPutShadowMap.frag");
	glUseProgram(ProgramId);

	// Bind depth texture to texture unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap.getDepthMap());
	glUniform1i(glGetUniformLocation(ProgramId, "depthMap"), 0);

	// Clear the screen before drawing
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);  // Render the quad with the depth map
	glBindVertexArray(0);  // Unbind VAO
}


void MapChunk::outputDepthValuesFromTexture() {
	float shadowWidth = WindowManager::get().getWidth() * 2, shadowHeight = WindowManager::get().getHeight() * 2;
	glBindFramebuffer(GL_FRAMEBUFFER, depthMap.getDepthMapFBO());
	std::vector<float> depthValues(shadowWidth * shadowHeight);
	glReadPixels(0, 0, shadowWidth, shadowHeight, GL_DEPTH_COMPONENT, GL_FLOAT, depthValues.data());
	int ct = 0;

	for (int y = 0; y < shadowHeight; ++y) {
		for (int x = 0; x < shadowWidth; ++x) {
			int idx = y * shadowWidth + x;
			float depthValue = depthValues[idx];
			if (depthValue < 1.0f) ct++;;
		}
	}
	std::cout << depthValues.size() << " " << ct << "\n";
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 MapChunk::getLightSpaceMatrix() 
{
	glm::mat4 lightProjection = glm::ortho(1.0f * -CHUNK_SIZE, 1.0f * CHUNK_SIZE, 1.0f * -CHUNK_SIZE, 1.0f * CHUNK_SIZE, 0.1f, 1000.0f);
	glm::vec3 obs = glm::vec3(x * CHUNK_SIZE + CHUNK_SIZE / 2, 0.0f, y * CHUNK_SIZE + CHUNK_SIZE / 2) + (directionalLight * -24.0f);
	glm::vec3 pctRef = glm::vec3(x * CHUNK_SIZE + CHUNK_SIZE / 2, 0.0f, y * CHUNK_SIZE + CHUNK_SIZE / 2);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 lightView = glm::lookAt(obs, pctRef, up);
	return lightProjection * lightView;

}

void MapChunk::renderShadowMap() 
{
	glm::mat4 lightSpaceMatrix = getLightSpaceMatrix();

	glUseProgram(DepthMap::getShader());

	depthMap.bindFBO();

	GLuint lightSpaceMatrixLocation = glGetUniformLocation(DepthMap::getShader(), "lightSpaceMatrix");
	glUniformMatrix4fv(lightSpaceMatrixLocation, 1, GL_FALSE, &lightSpaceMatrix[0][0]);


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

	// Grass
	if (isCameraInChunk())
	{
		if (!grass)
		{
			generateGrass();
		}

		grass->update();
		grass->draw();
	}
	else if (grass)
	{
		grass.reset();
	}

	// Trees
	for (int i = 0; i < this->trees.size(); ++i)
	{
		glm::vec3 treePos = this->trees[i].first;
		float treeAngle = this->trees[i].second.first;
		glm::vec3 treeScale = this->trees[i].second.second;

		glm::mat4 model = glm::translate(treePos) * glm::rotate(glm::radians(treeAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(treeScale);

		Map::get().getTree()->draw(DepthMap::getShader(), model, this, true);
	}

	// Boats
	for (int i = 0; i < this->boats.size(); ++i)
	{
		glm::vec3 boatPos = this->boats[i].first;
		float boatAngle = this->boats[i].second.first;
		glm::vec3 boatScale = this->boats[i].second.second;

		glm::mat4 model = glm::translate(boatPos) * glm::rotate(glm::radians(boatAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(boatScale);

		Map::get().getBoat()->draw(DepthMap::getShader(), model, this, true);
	}

	depthMap.unbindFBO(WindowManager::get().getWidth(), WindowManager::get().getHeight());

	//outPutShadowMap();
}



void MapChunk::draw(GLuint modelProgramID)
{
	glUseProgram(Map::get().getProgramId());

	// set value for view matrix
	glUniformMatrix4fv(glGetUniformLocation(Map::get().getProgramId(), "projection"), 1, GL_FALSE, &Camera::get().getProjectionMatrix()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(Map::get().getProgramId(), "view"), 1, GL_FALSE, &Camera::get().getViewMatrix()[0][0]);
	glUniform3fv(glGetUniformLocation(Map::get().getProgramId(), "directionalLight"), 1, &MapChunk::directionalLight[0]);
	glUniformMatrix4fv(glGetUniformLocation(Map::get().getProgramId(), "lightSpaceMatrix"), 1, GL_FALSE, &getLightSpaceMatrix()[0][0]);

	// set texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureManager::get().getTexture("grass1"));
	glUniform1i(glGetUniformLocation(Map::get().getProgramId(), "texture0"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TextureManager::get().getTexture("sand1"));
	glUniform1i(glGetUniformLocation(Map::get().getProgramId(), "texture1"), 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, depthMap.getDepthMap());
	glUniform1i(glGetUniformLocation(Map::get().getProgramId(), "depthMap"), 2);

	glUniform1f(glGetUniformLocation(Map::get().getProgramId(), "threshWaterGrass"), Water::getHeight() + Grass::getThresholdWaterGrass() / 2.0f);

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

	// Grass
	if (isCameraInChunk())
	{
		if (!grass)
		{
			generateGrass();
		}
		
		grass->update();
		grass->draw();
	}
	else if (grass)
	{
		grass.reset();
	}

	// Trees
	for (int i = 0; i < this->trees.size(); ++i)
	{
		glm::vec3 treePos = this->trees[i].first;
		float treeAngle = this->trees[i].second.first;
		glm::vec3 treeScale = this->trees[i].second.second;

		glm::mat4 model = glm::translate(treePos) * glm::rotate(glm::radians(treeAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(treeScale);

		Map::get().getTree()->draw(modelProgramID, model, this, false);
	}

	// Boats
	for (int i = 0; i < this->boats.size(); ++i)
	{
		glm::vec3 boatPos = this->boats[i].first;
		float boatAngle = this->boats[i].second.first;
		glm::vec3 boatScale = this->boats[i].second.second;

		glm::mat4 model = glm::translate(boatPos) * glm::rotate(glm::radians(boatAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(boatScale);

		Map::get().getBoat()->draw(modelProgramID, model, this, false);
	}
}

void MapChunk::update()
{

}

void MapChunk::commonUpdate()
{
	float directionalLightAngleDegrees = GlobalClock::get().getDeltaTime() * MapChunk::DAY_NIGHT_CYCLE_SPEED;

	glm::vec3 rotationAxis = glm::vec3(1.0f, 0.0f, 0.0f);

	glm::vec4 rotatedDirectionalLight = glm::rotate(glm::mat4(1.0f), glm::radians(directionalLightAngleDegrees), rotationAxis) * glm::vec4(MapChunk::directionalLight, 1.0f);

	MapChunk::directionalLight = glm::vec3(rotatedDirectionalLight);
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
const int MapChunk::GENERATE_GRASS_COUNTER = 25;
const int MapChunk::MAX_COORDINATE_Y = 666013;

const float MapChunk::DELTA_CULLING_SHADOW_MAPPING = 1.0f;
const float MapChunk::INF_HEIGHT = 1000000.0f;

glm::vec3 MapChunk::directionalLight = glm::vec3(0.0f, -1.0f, 0.0f); // incepe cu lumina
const float MapChunk::DAY_NIGHT_CYCLE_SPEED = 36.0f;

const float MapChunk::TREE_PROBABILITY = 0.05f;

const float MapChunk::GRASS_TREE_THRESH = 0.5f;

const float MapChunk::TREE_BASE_OFFSET = -0.5f;

const float MapChunk::TREE_MIN_SCALE = 0.25f;
const float MapChunk::TREE_MAX_SCALE = 0.75f;

const float MapChunk::BOAT_PROBABILITY = 0.01f;

const float MapChunk::BOAT_BASE_OFFSET = -0.25f;

const float MapChunk::BOAT_MIN_SCALE = 0.25f;
const float MapChunk::BOAT_MAX_SCALE = 0.5f;

const float MapChunk::WATER_BOAT_THRESH = -1.0f;

