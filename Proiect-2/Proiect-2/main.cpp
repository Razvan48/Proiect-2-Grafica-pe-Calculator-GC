// Codul sursa este adaptat dupa OpenGLBook.com

#include <windows.h>  // biblioteci care urmeaza sa fie incluse
#include <stdlib.h> // necesare pentru citirea shader-elor
#include <stdio.h>
#include <GL/glew.h> // glew apare inainte de freeglut
#include <GL/freeglut.h> // nu trebuie uitat freeglut.h
#include "loadShaders.h"

#include <iostream>
#include <unordered_map>

#include "Source/Camera/Camera.h"
#include "Source/Skybox/Skybox.h"
#include "Source/Model/Model.h"
#include "Source/GlobalClock/GlobalClock.h"

#include "Source/Map/Map.h"

// Window settings
const unsigned int SCR_WIDTH = 1400;
const unsigned int SCR_HEIGHT = 800;

// Camera settings
bool firstMouse = true;
float lastX = static_cast<float>(SCR_WIDTH) / 2.0f;
float lastY = static_cast<float>(SCR_HEIGHT) / 2.0f;

// Objects
Skybox* skybox;
Model* donut;

// Shaders
GLuint modelProgramID;

// Input
std::unordered_map<char, bool> keyStates;

// Callbacks
void updateFunction(int val)
{
	GlobalClock::get().update();

	if (keyStates['W'])
	{
		Camera::Get().ProcessKeyboard(FORWARD, GlobalClock::get().getDeltaTime());
	}

	if (keyStates['S'])
	{
		Camera::Get().ProcessKeyboard(BACKWARD, GlobalClock::get().getDeltaTime());
	}

	if (keyStates['A'])
	{
		Camera::Get().ProcessKeyboard(LEFT, GlobalClock::get().getDeltaTime());
	}

	if (keyStates['D'])
	{
		Camera::Get().ProcessKeyboard(RIGHT, GlobalClock::get().getDeltaTime());
	}

	if (keyStates['E'])
	{
		Camera::Get().ProcessKeyboard(UP, GlobalClock::get().getDeltaTime());
	}

	if (keyStates['Q'])
	{
		Camera::Get().ProcessKeyboard(DOWN, GlobalClock::get().getDeltaTime());
	}

	bool redraw = keyStates['W'] || keyStates['S'] || keyStates['A'] || keyStates['D'] || keyStates['E'] || keyStates['Q'];
	if (redraw)
	{
		glutPostRedisplay();
	}

	GlobalClock::get().update();
	glutTimerFunc(16, updateFunction, 0);
}

void processNormalKeysDown(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w':
	case 'W':
		keyStates['W'] = true;
		break;

	case 's':
	case 'S':
		keyStates['S'] = true;
		break;

	case 'a':
	case 'A':
		keyStates['A'] = true;
		break;

	case 'd':
	case 'D':
		keyStates['D'] = true;
		break;

	case 'e':
	case 'E':
		keyStates['E'] = true;
		break;

	case 'q':
	case 'Q':
		keyStates['Q'] = true;
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
		Camera::Get().SetMovementSpeed(5.0f);
	}
}

void processNormalKeysUp(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w':
	case 'W':
		keyStates['W'] = false;
		break;

	case 's':
	case 'S':
		keyStates['S'] = false;
		break;

	case 'a':
	case 'A':
		keyStates['A'] = false;
		break;

	case 'd':
	case 'D':
		keyStates['D'] = false;
		break;

	case 'e':
	case 'E':
		keyStates['E'] = false;
		break;

	case 'q':
	case 'Q':
		keyStates['Q'] = false;
		break;
	}
}

void processSpecialKeysUp(int key, int x, int y)
{
	if (key == GLUT_KEY_SHIFT_L || key == GLUT_KEY_SHIFT_R)
	{
		Camera::Get().SetMovementSpeed(2.0f);
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
		Camera::Get().ProcessMouseMovement(xoffset, yoffset);
		glutPostRedisplay();
	}
}

void processMouseKeys(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		if (button == 3)
		{
			Camera::Get().ProcessMouseScroll(1.0);
			glutPostRedisplay();
		}
		else if (button == 4)
		{
			Camera::Get().ProcessMouseScroll(-1.0);
			glutPostRedisplay();
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
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowPosition(100, 100); // pozitia initiala a ferestrei
	glutInitWindowSize(SCR_WIDTH, SCR_HEIGHT);
	glutCreateWindow("Grafica pe calculator - Proiect 2");

	glewInit();
	Initialize();

	glutDisplayFunc(RenderFunction);

	glutSetCursor(GLUT_CURSOR_NONE);

	glutTimerFunc(16, updateFunction, 0);

	glutKeyboardFunc(processNormalKeysDown);
	glutSpecialFunc(processSpecialKeysDown);
	glutKeyboardUpFunc(processNormalKeysUp);
	glutSpecialUpFunc(processSpecialKeysUp);
	glutPassiveMotionFunc(processMouseInput);
	glutMouseFunc(processMouseKeys);

	glutCloseFunc(Cleanup);
	glutMainLoop();
}
