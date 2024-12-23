#pragma once

#include "../WindowManager/WindowManager.h"

#include <unordered_map>

class InputManager
{
private:
	// Constructor
	InputManager();

	// Destructror
	~InputManager();

	// Not copyable
	InputManager(const InputManager& other) = delete;
	InputManager& operator= (const InputManager& other) = delete;

	// Not moveable
	InputManager(const InputManager&& other) = delete;
	InputManager& operator= (const InputManager&& other) = delete;

	static void keyboardFuncWrapper(unsigned char key, int x, int y);
	void processNormalKeysDown(unsigned char key, int x, int y);

	static void keyboardUpFuncWrapper(unsigned char key, int x, int y);
	void processNormalKeysUp(unsigned char key, int x, int y);

	static void specialFuncWrapper(int key, int x, int y);
	void processSpecialKeysDown(int key, int x, int y);

	static void specialUpFuncWrapper(int key, int x, int y);
	void processSpecialKeysUp(int key, int x, int y);

	static void passiveMotionFuncWrapper(int xpos, int ypos);
	void processMouseInput(int xpos, int ypos);

	static void mouseFuncWrapper(int button, int state, int x, int y);
	void processMouseKeys(int button, int state, int x, int y);

	std::unordered_map<char, bool> keyStates;

public:
	static InputManager& get();
	void update();
};

