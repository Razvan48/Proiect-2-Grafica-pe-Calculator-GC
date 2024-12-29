#pragma once

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "../Water/WaterFrameBuffers.h"

class textureRect
{
private:
	GLuint programId, VAO, VBO;
	WaterFrameBuffers fbos;

	void prepareTextures();

public:
	textureRect(const WaterFrameBuffers& fbos_);
	~textureRect();

	void draw(GLuint textureId, GLint posCenterX = 150, GLint posCenterY = 100, GLint width = 200, GLint height = 150);
	void CreateVAO();

};