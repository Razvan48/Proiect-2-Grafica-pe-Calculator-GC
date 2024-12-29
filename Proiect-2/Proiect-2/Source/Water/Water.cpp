#include "Water.h"

#include <glm/gtc/type_ptr.hpp>

#include "loadShaders.h"
#include "../Camera/Camera.h"
#include "../TextureManager/TextureManager.h"
#include "../GlobalClock/GlobalClock.h"
#include "../Map/Map.h"

#include <iostream>

Water::Water(const WaterFrameBuffers& fbos_, GLfloat height_, const std::string& dudvMapName_, 
			const std::string& normalMapName_, float waterSpeed_) 
	: height(height_), fbos(fbos_), dudvMapName(dudvMapName_), normalMapName(normalMapName_), waterSpeed(waterSpeed_)
{
	this->createVAO();
	this->programId = LoadShaders("shaders/water/water.vert", "shaders/water/water.frag");

	// glUniform1i(glGetUniformLocation(this->programId, "reflectionTexture"), 0);
	// glUniform1i(glGetUniformLocation(this->programId, "refractionTexture"), 1);
}

Water::~Water()
{
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VBO);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VAO);
}

void Water::createVAO()
{
	GLfloat Vertices[] = {
		-0.5f, 0.0f, -0.5f,    0.0f, 0.2f, 0.5f,    1.0f, 0.0f,
		-0.5f, 0.0f, 0.5f,    0.0f, 0.2f, 0.5f,    0.0f, 0.0f,
		0.5f, 0.0f, -0.5f,    0.0f, 0.2f, 0.5f,    1.0f, 1.0f,

		0.5f, 0.0f, -0.5f,    0.0f, 0.2f, 0.5f,    1.0f, 1.0f,
		-0.5f, 0.0f, 0.5f,    0.0f, 0.2f, 0.5f,    0.0f, 0.0f,
		0.5f, 0.0f, 0.5f,    0.0f, 0.2f, 0.5f,    0.0f, 1.0f
	};

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), &Vertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);  // atribut 1 = pozitie
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);  // atribut 2 = culoare
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
}

void Water::prepareRender(const glm::vec4& lightPos, const glm::vec3& lightColor)
{
	glUseProgram(this->programId);
	glBindVertexArray(VAO);
	// glEnableVertexAttribArray(0);
	
	// reflection texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fbos.getReflectionTexture());
	
	// refraction texture
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, fbos.getRefractionTexture());

	// dudvMap texture
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, TextureManager::get().getTexture(this->dudvMapName));

	// normal Map
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, TextureManager::get().getTexture(this->normalMapName));

	// depth map
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, fbos.getRefractionDepthTexture());

	glUniform1i(glGetUniformLocation(this->programId, "reflectionTexture"), 0);
	glUniform1i(glGetUniformLocation(this->programId, "refractionTexture"), 1);
	glUniform1i(glGetUniformLocation(this->programId, "dudvMap"), 2);
	glUniform1i(glGetUniformLocation(this->programId, "normalMap"), 3);
	glUniform1i(glGetUniformLocation(this->programId, "depthMap"), 4);

	moveFactor += this->waterSpeed * GlobalClock::get().getDeltaTime();
	moveFactor -= (float)((int)moveFactor);
	glUniform1f(glGetUniformLocation(this->programId, "moveFactor"), moveFactor);

	glUniform3fv(glGetUniformLocation(this->programId, "cameraPosition"), 1, glm::value_ptr(Camera::get().getPosition()));

	// light (position and color)
	glUniform4fv(glGetUniformLocation(this->programId, "lightPosition"), 1, glm::value_ptr(lightPos));
	glUniform3fv(glGetUniformLocation(this->programId, "lightColor"), 1, glm::value_ptr(lightColor));

	// depth mapping (far and near values)
	glUniform1f(glGetUniformLocation(this->programId, "near"), 0.1f); // de facut getteri in camera pentru near si far
	glUniform1f(glGetUniformLocation(this->programId, "far"), 1000.0f);

	
}

void Water::draw(const glm::vec4& lightPos, const glm::vec3& lightColor)
{
	// glUseProgram(this->programId);
	this->prepareRender(lightPos, lightColor);

	// projection
	glm::mat4 projection = Camera::get().getProjectionMatrix();
	glUniformMatrix4fv(glGetUniformLocation(programId, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	// view
	glm::mat4 view = Camera::get().getViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(programId, "view"), 1, GL_FALSE, glm::value_ptr(view));

	// model
	glm::vec2 Map_minXZ = Map::get().getMinXZ();
	glm::vec2 Map_maxXZ = Map::get().getMaxXZ() + glm::vec2(20.0, 20.0);
	GLfloat scaleFactorX = abs(Map_maxXZ.x - Map_minXZ.x);
	GLfloat scaleFactorZ = abs(Map_maxXZ.y - Map_minXZ.y);
	GLfloat posWaterX = scaleFactorX / 2.0;
	GLfloat posWaterZ = scaleFactorZ / 2.0;

	glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(scaleFactorX, 1.0f, scaleFactorZ));
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(posWaterX, this->height, posWaterZ)) * scale;
	glUniformMatrix4fv(glGetUniformLocation(programId, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(programId, "scale"), 1, GL_FALSE, glm::value_ptr(scale));

	// glBindVertexArray(VAO);



	GLboolean blendingEnabled;
	GLint sfactor, dfactor;

	glGetBooleanv(GL_BLEND, &blendingEnabled);
	glGetIntegerv(GL_BLEND_SRC, &sfactor);
	glGetIntegerv(GL_BLEND_DST, &dfactor);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	if (blendingEnabled == GL_FALSE)
		glDisable(GL_BLEND);
	else
		glEnable(GL_BLEND);
	glBlendFunc(sfactor, dfactor);

}
