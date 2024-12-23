#include "InputManager.h"

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <iostream>

#include "../Source/GlobalClock/GlobalClock.h"
#include "../Source/Camera/Camera.h"

InputManager::InputManager()
{
	// Callbacks
	glutKeyboardFunc(KeyboardFuncWrapper);
	glutKeyboardUpFunc(KeyboardUpFuncWrapper);

	glutSpecialFunc(SpecialFuncWrapper);
	glutSpecialUpFunc(SpecialUpFuncWrapper);

	glutPassiveMotionFunc(PassiveMotionFuncWrapper);
	glutMouseFunc(MouseFuncWrapper);
}

InputManager& InputManager::get()
{
	static InputManager instance;
	return instance;
}

InputManager::~InputManager()
{

}

void InputManager::update()
{
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
}

void InputManager::KeyboardFuncWrapper(unsigned char key, int x, int y)
{
	InputManager::get().processNormalKeysDown(key, x, y);
}

void InputManager::processNormalKeysDown(unsigned char key, int x, int y)
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

void InputManager::KeyboardUpFuncWrapper(unsigned char key, int x, int y)
{
	InputManager::get().processNormalKeysUp(key, x, y);
}

void InputManager::processNormalKeysUp(unsigned char key, int x, int y)
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

void InputManager::SpecialFuncWrapper(int key, int x, int y)
{
	InputManager::get().processSpecialKeysDown(key, x, y);
}

void InputManager::processSpecialKeysDown(int key, int x, int y)
{
	if (key == GLUT_KEY_SHIFT_L || key == GLUT_KEY_SHIFT_R)
	{
		Camera::Get().SetMovementSpeed(10.0f);
	}
}

void InputManager::SpecialUpFuncWrapper(int key, int x, int y)
{
	InputManager::get().processSpecialKeysUp(key, x, y);
}

void InputManager::processSpecialKeysUp(int key, int x, int y)
{
	if (key == GLUT_KEY_SHIFT_L || key == GLUT_KEY_SHIFT_R)
	{
		Camera::Get().SetMovementSpeed(5.0f);
	}
}

void InputManager::PassiveMotionFuncWrapper(int xpos, int ypos)
{
	InputManager::get().processMouseInput(xpos, ypos);
}

void InputManager::processMouseInput(int xpos, int ypos)
{
	static bool firstMouse = true;
	static float lastX = static_cast<float>(WindowManager::get().getWidth()) / 2.0f;
	static float lastY = static_cast<float>(WindowManager::get().getHeight()) / 2.0f;

	if (firstMouse)
	{
		firstMouse = false;
		glutWarpPointer(WindowManager::get().getWidth() / 2, WindowManager::get().getHeight() / 2);
		return;
	}

	int xoffset = xpos - WindowManager::get().getWidth() / 2;
	int yoffset = WindowManager::get().getHeight() / 2 - ypos;

	if (xoffset != 0 || yoffset != 0)
	{
		glutWarpPointer(WindowManager::get().getWidth() / 2, WindowManager::get().getHeight() / 2);
		Camera::Get().ProcessMouseMovement(static_cast<float>(xoffset), static_cast<float>(yoffset));
		glutPostRedisplay();
	}
}

void InputManager::MouseFuncWrapper(int button, int state, int x, int y)
{
	InputManager::get().processMouseKeys(button, state, x, y);
}

void InputManager::processMouseKeys(int button, int state, int x, int y)
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
