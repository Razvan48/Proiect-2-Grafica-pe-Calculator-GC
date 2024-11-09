#pragma once

#include <string>
#include <map>

#include <GL/glew.h>
#include <GL/freeglut.h>

class TextureManager
{
private:
	TextureManager();
	~TextureManager();
	TextureManager(const TextureManager& other) = delete;
	TextureManager& operator= (const TextureManager& other) = delete;
	TextureManager(const TextureManager&& other) = delete;
	TextureManager& operator= (const TextureManager&& other) = delete;

	struct Texture
	{
		GLuint texture;
		int width;
		int height;
		int nrChannels;
	};
	std::map<std::string, Texture> textures;

	void loadTexture(const std::string& textureName, const std::string& texturePath);

public:
	static TextureManager& get();
	void loadResources();
	GLuint getTexture(const std::string& textureName);
};

