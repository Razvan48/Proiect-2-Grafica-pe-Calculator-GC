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
#include "Source/WaterTesters/textureRect.h"
#include "Source/Water/WaterFrameBuffers.h"
#include "Source/Water/Water.h"


// Objects
Skybox* skybox;
Model* donut;
textureRect *reflectionPanel, *refractionPanel;
WaterFrameBuffers* fbos;
Water* water;

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

void setClippingPlane(GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
	GLint programInUse;
	glGetIntegerv(GL_CURRENT_PROGRAM, &programInUse);

	glUseProgram(Map::get().getProgramId());
	glUniform4f(glGetUniformLocation(Map::get().getProgramId(), "plane"), x, y, z, w);

	// glUseProgram(skybox->getProgramId());
	// glUniform4f(glGetUniformLocation(skybox->getProgramId(), "plane"), x, y, z, w);

	glUseProgram(programInUse);
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
	fbos = new WaterFrameBuffers(WindowManager::get().getWidth(), WindowManager::get().getHeight());
	reflectionPanel = new textureRect(*fbos);
	refractionPanel = new textureRect(*fbos);
	water = new Water(*fbos);
}

void RenderFunction(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw objects
	donut->draw(modelProgramID);

	// Draw skybox as last
	skybox->draw();

	Map::get().draw();
	Map::get().update(); // TODO: de mutat de aici (momentan doar sa mearga)

	water->draw();
	



	// ===== for water rendering

	glEnable(GL_CLIP_DISTANCE0);
	// create the reflection of all objects
	fbos->bindReflectionFrameBuffer();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Repositioning of the camera for the reflection
	GLfloat distance = 2 * (Camera::get().getPosition().y - water->getHeight());
	glm::vec3 cameraPos = Camera::get().getPosition();
	cameraPos.y -= distance;
	Camera::get().setPosition(cameraPos);
	Camera::get().invertPitch();
	setClippingPlane(0, 1, 0, -water->getHeight() + 0.02f);

	// Draw objects
	donut->draw(modelProgramID);

	// Draw skybox as last
	skybox->draw();
	// Draw map
	Map::get().draw();
	// water --------------------
	// water->draw();
	// --------------

	// Bring camera to the initial position
	cameraPos.y += distance;
	Camera::get().setPosition(cameraPos);
	Camera::get().invertPitch();

	fbos->unbindCurrentFrameBuffer();

	// end create reflection


	// create the refraction of all objects
	fbos->bindRefractionFrameBuffer();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	setClippingPlane(0, -1, 0, water->getHeight() + 0.1f);

	// Draw objects
	donut->draw(modelProgramID);

	// Draw skybox as last
	skybox->draw();
	// Draw map
	Map::get().draw();

	// water ----------------
	// water->draw();
	// -------------

	fbos->unbindCurrentFrameBuffer();
	// end create refraction

	glDisable(GL_CLIP_PLANE0);
	// setClippingPlane(0, 1, 0, 1000000); // daca nu merge disable la clip_plane

	// reflectionPanel->draw(fbos->getReflectionTexture());
	// refractionPanel->draw(fbos->getRefractionTexture(), 400);

	// ===== end for water rendering




	glutSwapBuffers();
	glFlush();

	glutPostRedisplay();
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
