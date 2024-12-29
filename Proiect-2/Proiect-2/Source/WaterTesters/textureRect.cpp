#include "textureRect.h"

#include "loadShaders.h"
#include "../WindowManager/WindowManager.h"


textureRect::textureRect(const WaterFrameBuffers& fbos_) 
	: fbos(fbos_)
{
	this->programId = this->programId = LoadShaders("shaders/waterTesters/textureRect.vert", "shaders/waterTesters/textureRect.frag");
	this->CreateVAO();

	glUniform1i(glGetUniformLocation(this->programId, "ourTexture"), 0);
}

textureRect::~textureRect()
{
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VBO);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VAO);

	// Delete shaders
	glDeleteProgram(programId);

	// Delete texture
	// glDeleteTextures(1, &textureID);
}

void textureRect::prepareTextures()
{
	glUseProgram(this->programId);
	glBindVertexArray(VAO);
	// glEnableVertexAttribArray(2);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fbos.getReflectionTexture());
}

void textureRect::draw(GLuint textureId, GLint posCenterX, GLint posCenterY, GLint width, GLint height)
{
	glUseProgram(this->programId);
	// this->prepareTextures();

	// projection
	glm::mat4 projection = glm::ortho(0.0f, (float)WindowManager::get().getWidth(), 0.0f, (float)WindowManager::get().getHeight(), -1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(programId, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	// view
	glm::mat4 view = glm::mat4(1.0f);
	glUniformMatrix4fv(glGetUniformLocation(programId, "view"), 1, GL_FALSE, glm::value_ptr(view));

	// model
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3((float)posCenterX, (float)(WindowManager::get().getHeight()-posCenterY), 0.0f))
		* glm::scale(glm::mat4(1.0f), glm::vec3((float)width, (float)height, 0.0f));

	glUniformMatrix4fv(glGetUniformLocation(programId, "model"), 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(VAO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId);

	glDisable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glEnable(GL_DEPTH_TEST);
}

void textureRect::CreateVAO()
{
	GLfloat Vertices[] = {
	-0.5f, -0.5f, 0.0f,    0.0f, 0.0f, 1.0f,    -0.1f, -0.1f,
	-0.5f, 0.5f, 0.0f,    0.0f, 0.0f, 1.0f,    -0.1f, 1.1f,
	0.5f, -0.5f, 0.0f,    0.0f, 0.0f, 1.0f,    1.1f, -0.1f,

	0.5f, -0.5f, 0.0f,    0.0f, 0.0f, 1.0f,    1.1f, -0.1f,
	-0.5f, 0.5f, 0.0f,    0.0f, 0.0f, 1.0f,    -0.1f, 1.1f,
	0.5f, 0.5f, 0.0f,    0.0f, 0.0f, 1.0f,    1.1f, 1.1f
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


