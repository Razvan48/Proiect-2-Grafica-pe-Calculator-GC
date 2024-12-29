#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

class WaterFrameBuffers
{
public:
	//int ReflectionWidth = 320;
	//int ReflectionHeight = 180;

	int ReflectionWidth = 0;
	int ReflectionHeight = 0;

	int RefractionWidth = 0;
	int RefractionHeight = 0;

	GLuint reflectionFrameBuffer;
	GLuint reflectionTexture;
	GLuint reflectionDepthBuffer;

	GLuint refractionFrameBuffer;
	GLuint refractionTexture;
	GLuint refractionDepthTexture;

public:
	WaterFrameBuffers(int WindowWidth, int WindowHeight);
	~WaterFrameBuffers();

	inline void bindReflectionFrameBuffer() { bindFrameBuffer(reflectionFrameBuffer, ReflectionWidth, ReflectionHeight); }
	void bindRefractionFrameBuffer() { bindFrameBuffer(refractionFrameBuffer, RefractionWidth, RefractionHeight); }

	void unbindCurrentFrameBuffer();

	inline GLuint getReflectionTexture() const { return reflectionTexture; }
	inline GLuint getRefractionTexture() const { return refractionTexture; }

	inline GLuint getRefractionDepthTexture() const { return refractionDepthTexture; }

	void initializeReflectionFrameBuffer();
	void initializeRefractionFrameBuffer();

	void bindFrameBuffer(GLuint frameBuffer, int width, int height);
	GLuint createFrameBuffer();
	GLuint createTextureAttachment(int width, int height);
	GLuint createDepthTextureAttachment(int width, int height);
	GLuint createDepthBufferAttachment(int width, int height);

};