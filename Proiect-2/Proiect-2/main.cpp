// Codul sursa este adaptat dupa OpenGLBook.com

#include <windows.h>  // biblioteci care urmeaza sa fie incluse
#include <stdlib.h> // necesare pentru citirea shader-elor
#include <stdio.h>
#include <GL/glew.h> // glew apare inainte de freeglut
#include <GL/freeglut.h> // nu trebuie uitat freeglut.h
#include "loadShaders.h"

#include <iostream>

#include <thread>

#include "Source/WindowManager/WindowManager.h"
#include "Source/InputManager/InputManager.h"
#include "Source/TextureManager/TextureManager.h"
#include "Source/GlobalClock/GlobalClock.h"
#include "Source/Camera/Camera.h"
#include "Source/Skybox/Skybox.h"
#include "Source/Map/Map.h"
#include "Source/Model/Model.h"
#include "Source/Water/WaterFrameBuffers.h"
#include "Source/Water/Water.h"

// Objects
Skybox* skybox;
Model* donut;
Model* tree;
WaterFrameBuffers* fbos;
Water* water;

// Shaders
GLuint modelProgramID;

// Uniform buffers
GLuint cameraUniformBuffer;

void updateFunction(int val)
{
	// Input
	InputManager::get().update();

	// Map - Terrain + Grass
	Map::get().update();

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

void setClippingPlane(GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
	GLint programInUse;
	glGetIntegerv(GL_CURRENT_PROGRAM, &programInUse);

	glUseProgram(Map::get().getProgramId());
	glUniform4f(glGetUniformLocation(Map::get().getProgramId(), "plane"), x, y, z, w);

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

	// Uniform buffers
	glGenBuffers(1, &cameraUniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, cameraUniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, 128, nullptr, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, cameraUniformBuffer);

	// Objects
	donut = new Model("resources/donut/tor.obj");
	tree = new Model("resources/tree/tree0.obj");
	skybox = new Skybox();

	Grass::setupOpenGL();

	fbos = new WaterFrameBuffers(WindowManager::get().getWidth(), WindowManager::get().getHeight());
	water = new Water(*fbos);

	Camera::get().setPosition(glm::vec3(0.0f, 25.0f, 0.0f));
}

void drawAllObjectsExceptWater()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw objects
	// donut->draw(modelProgramID);

	// tree
	tree->draw(modelProgramID);

	// Grass
	glm::mat4 view = Camera::get().getViewMatrix();
	glm::mat4 projection = Camera::get().getProjectionMatrix();

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, cameraUniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, cameraUniformBuffer);

	glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, &view);
	glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, &projection);

	// Map - Terrain + Grass
	Map::get().draw();

	// Draw skybox as last
	skybox->draw();
}

void computeWater()
{
	glEnable(GL_CLIP_DISTANCE0);
	// create the reflection of all objects
	fbos->bindReflectionFrameBuffer();

	// Repositioning of the camera for the reflection
	GLfloat distance = 2 * (Camera::get().getPosition().y - water->getHeight());
	glm::vec3 cameraPos = Camera::get().getPosition();
	cameraPos.y -= distance;
	Camera::get().setPosition(cameraPos);
	Camera::get().invertPitch();
	setClippingPlane(0, 1, 0, -water->getHeight() + 0.02f);

	// Draw objects
	drawAllObjectsExceptWater();

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
	drawAllObjectsExceptWater();

	fbos->unbindCurrentFrameBuffer();
	// end create refraction

	glDisable(GL_CLIP_PLANE0);
	// setClippingPlane(0, 1, 0, 1000000); // daca nu merge disable la clip_plane

}

void RenderFunction(void)
{
	drawAllObjectsExceptWater();

	computeWater();
	water->draw(glm::vec4(MapChunk::getDirectionalLight(), 0.0f));

	glutSwapBuffers();
	glFlush();
}

void Cleanup(void)
{
	DestroyShaders();

	// Objects
	delete skybox;
	delete donut;
	delete tree;
	delete water;
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
