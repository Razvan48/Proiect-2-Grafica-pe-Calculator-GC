// Codul sursa este adaptat dupa OpenGLBook.com

#include <windows.h>  // biblioteci care urmeaza sa fie incluse
#include <stdlib.h> // necesare pentru citirea shader-elor
#include <stdio.h>
#include <GL/glew.h> // glew apare inainte de freeglut
#include <GL/freeglut.h> // nu trebuie uitat freeglut.h
#include "loadShaders.h"

#include <iostream>

#include "Source/Camera/Camera.h"
#include "Source/Skybox/Skybox.h"
#include "Source/Model/Model.h"

// Window settings
const unsigned int SCR_WIDTH = 1400;
const unsigned int SCR_HEIGHT = 800;

// Global clock
const float deltaTime = 0.11f; // TODO: TEST - de folosit global clock

// Camera settings
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool firstMouse = true;
float lastX = static_cast<float>(SCR_WIDTH) / 2.0f;
float lastY = static_cast<float>(SCR_HEIGHT) / 2.0f;

// Objects
Skybox* skybox;
Model* donut;

// Shaders
GLuint modelProgramID;

// Callbacks
void processNormalKeysDown(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w':
	case 'W':
		camera.ProcessKeyboard(FORWARD, deltaTime);
		break;

	case 's':
	case 'S':
		camera.ProcessKeyboard(BACKWARD, deltaTime);
		break;

	case 'a':
	case 'A':
		camera.ProcessKeyboard(LEFT, deltaTime);
		break;

	case 'd':
	case 'D':
		camera.ProcessKeyboard(RIGHT, deltaTime);
		break;

	case 'e':
	case 'E':
		camera.ProcessKeyboard(UP, deltaTime);
		break;

	case 'q':
	case 'Q':
		camera.ProcessKeyboard(DOWN, deltaTime);
		break;
	}

	// ESC
	if (key == 27)
	{
		exit(0);
	}
}

void processSpecialKeysDown(int key, int x, int y)
{
	if (key == GLUT_KEY_SHIFT_L || key == GLUT_KEY_SHIFT_R)
	{
		std::cout << "SHIFT DOWN" << std::endl;
		camera.SetMovementSpeed(5.0f);
	}
}

void processNormalKeysUp(unsigned char key, int x, int y)
{
	// TODO
}

void processSpecialKeysUp(int key, int x, int y)
{
	if (key == GLUT_KEY_SHIFT_L || key == GLUT_KEY_SHIFT_R)
	{
		std::cout << "SHIFT UP" << std::endl;
		camera.SetMovementSpeed(2.0f);
	}
}

void processMouseInput(int xpos, int ypos)
{
	if (firstMouse)
	{
		firstMouse = false;
		glutWarpPointer(SCR_WIDTH / 2, SCR_HEIGHT / 2);
		return;
	}

	int xoffset = xpos - SCR_WIDTH / 2;
	int yoffset = SCR_HEIGHT / 2 - ypos;

	if (xoffset != 0 || yoffset != 0)
	{
		glutWarpPointer(SCR_WIDTH / 2, SCR_HEIGHT / 2);
		camera.ProcessMouseMovement(xoffset, yoffset);
		glutPostRedisplay();
	}
}

void processMouseKeys(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		if (button == 3)
		{
			camera.ProcessMouseScroll(1.0);
		}
		else if (button == 4)
		{
			camera.ProcessMouseScroll(-1.0);
		}
	}
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

	static int i = 0;
	std::cout << "Render " << i++ << std::endl;
	if (i > 3) i = 0;

	// Draw objects
	donut->Render(camera, modelProgramID);

	// Draw skybox as last
	skybox->Render(camera);

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
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowPosition(100, 100); // pozitia initiala a ferestrei
	glutInitWindowSize(SCR_WIDTH, SCR_HEIGHT);
	glutCreateWindow("Grafica pe calculator - Proiect 2");

	glewInit();
	Initialize();

	glutDisplayFunc(RenderFunction);

	glutSetCursor(GLUT_CURSOR_NONE);

	glutKeyboardFunc(processNormalKeysDown);
	glutSpecialFunc(processSpecialKeysDown);
	glutKeyboardUpFunc(processNormalKeysUp);
	glutSpecialUpFunc(processSpecialKeysUp);
	glutPassiveMotionFunc(processMouseInput);
	glutMouseFunc(processMouseKeys);

	glutCloseFunc(Cleanup);
	glutMainLoop();
}
