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

// Callbacks
void processNormalKeys(unsigned char key, int x, int y)
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

void processSpecialKeys(int key, int x, int y)
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
	// TODO
	//ProgramId = LoadShaders("example.vert", "example.frag");
	//glUseProgram(ProgramId);
}

void DestroyShaders(void)
{
	// TODO
	//glDeleteProgram(ProgramId);
}

void Initialize(void)
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // culoarea de fond a ecranului
	CreateShaders();

	// Objects
	skybox = new Skybox();
}

void RenderFunction(void)
{
	glClear(GL_COLOR_BUFFER_BIT);       

	// Functiile de desenare
	// TODO

	// Draw skybox as last
	skybox->Render(camera);

	glFlush();
}

void Cleanup(void)
{
	DestroyShaders();

	// Objects
	delete skybox;
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(100, 100); // pozitia initiala a ferestrei
	glutInitWindowSize(SCR_WIDTH, SCR_HEIGHT);
	glutCreateWindow("Grafica pe calculator - Proiect 2");

	glewInit();
	Initialize();

	glutDisplayFunc(RenderFunction);

	glutSetCursor(GLUT_CURSOR_NONE);

	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(processSpecialKeys);
	glutKeyboardUpFunc(processNormalKeysUp);
	glutSpecialUpFunc(processSpecialKeysUp);
	glutPassiveMotionFunc(processMouseInput);
	glutMouseFunc(processMouseKeys);

	glutCloseFunc(Cleanup);
	glutMainLoop();
}
