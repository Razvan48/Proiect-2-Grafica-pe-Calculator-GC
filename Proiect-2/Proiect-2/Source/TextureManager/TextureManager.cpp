#include "TextureManager.h"

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <SOIL.h>

#include <iostream>

TextureManager::TextureManager()
{

}

TextureManager::~TextureManager()
{

}

TextureManager& TextureManager::get()
{
	static TextureManager instance;
	return instance;
}

void TextureManager::loadTexture(const std::string& textureName, const std::string& texturePath)
{
	if (this->textures.find(textureName) != this->textures.end())
		std::cout << "Warning: Texture " + textureName + " already loaded" << std::endl;
	else
		this->textures[textureName] = TextureManager::Texture();

	glGenTextures(1, &this->textures[textureName].texture);
	glBindTexture(GL_TEXTURE_2D, this->textures[textureName].texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	unsigned char* textureData = SOIL_load_image(texturePath.c_str(),
		&this->textures[textureName].width,
		&this->textures[textureName].height,
		&this->textures[textureName].nrChannels,
		SOIL_LOAD_RGBA);

	if (!textureData)
	{
		std::cout << "Error: Failed to load texture " + textureName + " at path : " << texturePath << std::endl;
		std::cout << "SOIL error: " << SOIL_last_result() << std::endl;
	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->textures[textureName].width, this->textures[textureName].height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	SOIL_free_image_data(textureData);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void TextureManager::loadResources()
{
	// TODO: Incarcam texturile aici + desenam/descarcam texturi pentru aplicatie.

	this->loadTexture("grass0", "resources/grass/grass0.jpg");
	this->loadTexture("grass1", "resources/grass/grass1.jpg");
}

GLuint TextureManager::getTexture(const std::string& textureName)
{
	if (this->textures.find(textureName) == this->textures.end())
	{
		std::cout << "Error: Texture " + textureName + " not found" << std::endl;
		return 0;
	}

	return this->textures[textureName].texture;
}

