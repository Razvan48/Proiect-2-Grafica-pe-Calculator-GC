// Codul sursa este adaptat dupa OpenGLBook.com

#include <windows.h>  // biblioteci care urmeaza sa fie incluse
#include <stdlib.h> // necesare pentru citirea shader-elor
#include <stdio.h>
#include <GL/glew.h> // glew apare inainte de freeglut
#include <GL/freeglut.h> // nu trebuie uitat freeglut.h
#include "loadShaders.h"

#include <iostream>

#include "Source/WindowManager/WindowManager.h"
#include "Source/InputManager/InputManager.h"
#include "Source/TextureManager/TextureManager.h"
#include "Source/GlobalClock/GlobalClock.h"
#include "Source/Camera/Camera.h"
#include "Source/Skybox/Skybox.h"
#include "Source/Map/Map.h"
#include "Source/Model/Model.h"
#include "Source/Grass/Grass.h"

// Objects
Skybox* skybox;
Model* donut;
Grass* grass;

// Shaders
GLuint modelProgramID;

// Uniform buffers
GLuint cameraUniformBuffer;

void updateFunction(int val)
{
	// Input
	InputManager::get().update();

	// Grass
	grass->update();

	// Clock
	GlobalClock::get().update();

	// Redraw
	glutPostRedisplay();

	// Update function
	glutTimerFunc(16, updateFunction, 0);
}

void CreateShaders(void)
{
	modelProgramID = LoadShaders("shaders/model/model.vert", "shaders/model/model.frag");
}

void DestroyShaders(void)
{
	glDeleteProgram(modelProgramID);
}

void Initialize(void)
{
	// Configure global opengl state
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	// Shaders
	CreateShaders();

	// Uniform buffers
	glGenBuffers(1, &cameraUniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, cameraUniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, 128, nullptr, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, cameraUniformBuffer);

	// Objects
	donut = new Model("resources/donut/tor.obj");
	skybox = new Skybox();
	grass = new Grass();
}

void RenderFunction(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw objects
	// donut->draw(modelProgramID);

	// Grass
	glm::mat4 view = Camera::get().getViewMatrix();
	glm::mat4 projection = Camera::get().getProjectionMatrix();

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, cameraUniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, cameraUniformBuffer);

	glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, &view);
	glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, &projection);
	
	grass->draw();

	// Draw skybox as last
	skybox->draw();

	Map::get().draw();
	Map::get().update(); // TODO: de mutat de aici (momentan doar sa mearga)

	glutSwapBuffers();
	glFlush();
}

void Cleanup(void)
{
	DestroyShaders();

	// Objects
	delete grass;
	delete skybox;
	delete donut;
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	
	// Init Window
	WindowManager::get();

	// TODO OpenGL minim 4.5 pentru Tessellation
	const GLubyte* version = glGetString(GL_VERSION);
	std::cout << "OpenGL version: " << version << std::endl;

	// Init Input Manager
	InputManager::get();

	Initialize();

	TextureManager::get().loadResources();

	glutDisplayFunc(RenderFunction);

	// Update function
	glutTimerFunc(16, updateFunction, 0);

	glutCloseFunc(Cleanup);
	glutMainLoop();
}
