#include "Skybox.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "loadShaders.h"
#include "stb_image.h"

#include "../Source/Camera/Camera.h"

const std::vector<std::string> Skybox::faces = {
	"resources/cubemap/posx.jpg",
	"resources/cubemap/negx.jpg",
	"resources/cubemap/posy.jpg",
	"resources/cubemap/negy.jpg",
	"resources/cubemap/posz.jpg",
	"resources/cubemap/negz.jpg"
};

Skybox::Skybox()
{
	loadCubemap();
	loadShadersSkybox();
	createVAO();
}

Skybox::~Skybox()
{
	// Delete VAO + VBO
	glDisableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VBO);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VAO);

	// Delete shaders
	glDeleteProgram(programId);

	// Delete texture
	glDeleteTextures(1, &cubemapTextureID);
}

void Skybox::draw()
{
	glUseProgram(programId);

	glm::mat4 view = glm::mat4(glm::mat3(Camera::get().getViewMatrix())); // remove translation from the view matrix
	glUniformMatrix4fv(glGetUniformLocation(programId, "view"), 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 projection = Camera::get().getProjectionMatrix();
	glUniformMatrix4fv(glGetUniformLocation(programId, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glBindVertexArray(VAO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextureID);

	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void Skybox::loadCubemap()
{
	glGenTextures(1, &cubemapTextureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextureID);

	int width, height, nrChannels;
	unsigned char* data;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			std::cout << "Failed to load cubemap: " << faces[i] << std::endl;
		}
		
		stbi_image_free(data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Skybox::loadShadersSkybox()
{
	programId = LoadShaders("shaders/skybox/skybox.vert", "shaders/skybox/skybox.frag");
	glUseProgram(programId);

	// Set skybox samplerCube to GL_TEXTURE0
	glUniform1i(glGetUniformLocation(programId, "skybox"), 0);
}

void Skybox::createVAO()
{
	static const float vertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}
