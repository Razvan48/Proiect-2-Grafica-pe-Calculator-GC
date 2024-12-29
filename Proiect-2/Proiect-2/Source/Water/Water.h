#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <vector>
#include <string>

#include "WaterFrameBuffers.h"

class Water
{
private:
	GLuint VAO, VBO, programId;
	GLfloat height;
	WaterFrameBuffers fbos;

	std::string dudvMapName = "";
	std::string normalMapName = "";

	float waterSpeed;
	float moveFactor = 0.0f;

	void createVAO();
	void prepareRender(const glm::vec4& lightPos, const glm::vec3& lightColor);

public:
	Water(const WaterFrameBuffers& fbos_, GLfloat height_ = 5.0f, const std::string& dudvMapName_ = "dudvMap", 
				const std::string& normalMapName_ = "waterNormalMap", float waterSpeed_ = 0.005f);
	~Water();
	void draw(const glm::vec4& lightPos = glm::vec4(-100.0f, -100.0, -100.0f, 0.0f), const glm::vec3& lightColor = glm::vec3(1.0f, 1.0f, 1.0f));

	// getters
	inline GLfloat getHeight() const { return this->height; }
	inline WaterFrameBuffers getfbos() const { return fbos; }
};