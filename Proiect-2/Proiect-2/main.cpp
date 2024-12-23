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

// Objects
Skybox* skybox;
Model* donut;

// Shaders
GLuint modelProgramID;

void updateFunction(int val)
{
	// Input
	InputManager::get().update();

	// Clock
	GlobalClock::get().update();

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

	// Objects
	donut = new Model("resources/donut/tor.obj");
	skybox = new Skybox();
}

void RenderFunction(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw objects
	donut->Render(modelProgramID);

	// Draw skybox as last
	skybox->Render();

	Map::get().draw();
	Map::get().update(); // TODO: de mutat de aici (momentan doar sa mearga)

	glutSwapBuffers();
	glFlush();
}

void Cleanup(void)
{
	DestroyShaders();

	// Objects
	delete skybox;
	delete donut;
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	
	// Init Window
	WindowManager::get();

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
