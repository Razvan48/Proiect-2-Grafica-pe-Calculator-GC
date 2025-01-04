#include "WaterFrameBuffers.h"

#include "../WindowManager/WindowManager.h"

WaterFrameBuffers::WaterFrameBuffers(int WindowWidth, int WindowHeight)
	: ReflectionWidth(WindowWidth), ReflectionHeight(WindowHeight),
	  RefractionWidth(WindowWidth), RefractionHeight(WindowHeight)
{
	initializeReflectionFrameBuffer();
	initializeRefractionFrameBuffer();
}

WaterFrameBuffers::~WaterFrameBuffers()
{
	glDeleteFramebuffers(1, &reflectionFrameBuffer);
	glDeleteFramebuffers(1, &reflectionFrameBuffer);
	glDeleteRenderbuffers(1, &reflectionDepthBuffer);

	glDeleteFramebuffers(1, &refractionFrameBuffer);
	glDeleteTextures(1, &refractionTexture);
	glDeleteTextures(1, &refractionDepthTexture);
}

void WaterFrameBuffers::unbindCurrentFrameBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// glViewport(0, 0, WindowManager::get().getWidth(), WindowManager::get().getHeight());
}

void WaterFrameBuffers::initializeReflectionFrameBuffer()
{
	reflectionFrameBuffer = createFrameBuffer();
	reflectionTexture = createTextureAttachment(ReflectionWidth, ReflectionHeight);
	reflectionDepthBuffer = createDepthBufferAttachment(ReflectionWidth, ReflectionHeight);
	unbindCurrentFrameBuffer();
}

void WaterFrameBuffers::initializeRefractionFrameBuffer()
{
	refractionFrameBuffer = createFrameBuffer();
	refractionTexture = createTextureAttachment(RefractionWidth, RefractionHeight);
	refractionDepthTexture = createDepthTextureAttachment(RefractionWidth, RefractionHeight);
	unbindCurrentFrameBuffer();
}

void WaterFrameBuffers::bindFrameBuffer(GLuint frameBuffer, int width, int height)
{
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	// glViewport(0, 0, width, height);
}

GLuint WaterFrameBuffers::createFrameBuffer()
{
	GLuint frameBuffer = 0;
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	return frameBuffer;
}

GLuint WaterFrameBuffers::createTextureAttachment(int vidth, int height)
{
	GLuint texture = 0;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, vidth, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//GLfloat borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);
	return texture;
}

GLuint WaterFrameBuffers::createDepthTextureAttachment(int vidth, int height)
{
	GLuint texture = 0;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, vidth, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture, 0);
	return texture;
}

GLuint WaterFrameBuffers::createDepthBufferAttachment(int width, int height)
{
	GLuint depthBuffer = 0;
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
	return depthBuffer;
}
