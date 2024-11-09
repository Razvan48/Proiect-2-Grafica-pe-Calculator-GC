#include "InputManager.h"

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <iostream>

InputManager::InputManager(bool leftMouseButtonUp, bool leftMouseButtonHold, int currentMouseX, int currentMouseY)
	: leftMouseButtonUp(leftMouseButtonUp), leftMouseButtonHold(leftMouseButtonHold), currentMouseX(currentMouseX), currentMouseY(currentMouseY)
{
	glutMouseFunc(mouseFuncWrapper);
	glutPassiveMotionFunc(motionFuncWrapper);
	glutMotionFunc(motionFuncWrapper);
}

InputManager& InputManager::get()
{
	static InputManager instance(false, false, 0, 0);
	return instance;
}

InputManager::~InputManager()
{

}

void InputManager::mouseFuncWrapper(int button, int state, int x, int y)
{
	InputManager::get().mouseFunc(button, state, x, y);
}

void InputManager::motionFuncWrapper(int x, int y)
{
	InputManager::get().motionFunc(x, y);
}

void InputManager::mouseFunc(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_UP)
		{
			this->leftMouseButtonUp = true;
			this->leftMouseButtonHold = false;
		}
		else if (state == GLUT_DOWN)
		{
			this->leftMouseButtonUp = false;
			this->leftMouseButtonHold = true;
		}
	}
}

void InputManager::motionFunc(int x, int y)
{
	this->currentMouseX = x;
	this->currentMouseY = y;
}

void InputManager::update()
{
	this->leftMouseButtonUp = false;
}

