#include "Model.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "objloader.h"	// TODO: de inlocuit

#include "../Source/Camera/Camera.h"

Model::Model(const std::string& objFilePath)
{
	bool model = loadOBJ("resources/donut/tor.obj", vertices, uvs, normals); // TODO: de inlocuit

	if (model)
	{
		CreateVAO();
	}
	else
	{
		// TODO: throw exception
	}
}

Model::~Model()
{
	// Delete VAO + VBO
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VBO);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VAO);

	// Delete shaders
	// glDeleteProgram(programId);

	// Delete texture
	// glDeleteTextures(1, &textureID);
}

void Model::Render(const GLuint& programId)
{
	glUseProgram(programId);

	// projection
	glm::mat4 projection = Camera::Get().GetProjectionMatrix();
	glUniformMatrix4fv(glGetUniformLocation(programId, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	// view
	glm::mat4 view = Camera::Get().GetViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(programId, "view"), 1, GL_FALSE, glm::value_ptr(view));

	// model
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));
	glUniformMatrix4fv(glGetUniformLocation(programId, "model"), 1, GL_FALSE, glm::value_ptr(model));

	// Set textureDiffuse sampler2D to GL_TEXTURE0
	glUniform1i(glGetUniformLocation(programId, "textureDiffuse"), 0);

	glBindVertexArray(VAO);

	// TODO
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, textureID);

	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
}

void Model::CreateVAO()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3) + normals.size() * sizeof(glm::vec3) + uvs.size() * sizeof(glm::vec2), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(glm::vec3), &vertices[0]);
	glBufferSubData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), normals.size() * sizeof(glm::vec3), &normals[0]);
	glBufferSubData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3) + normals.size() * sizeof(glm::vec3), uvs.size() * sizeof(glm::vec2), &uvs[0]);

	glEnableVertexAttribArray(0); // atributul 0 = pozitie
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

	glEnableVertexAttribArray(1); // atributul 1 = normale
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(vertices.size() * sizeof(glm::vec3)));

	glEnableVertexAttribArray(2); // atributul 2 = coordonate de texturare
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(vertices.size() * sizeof(glm::vec3) + normals.size() * sizeof(glm::vec3)));
}
