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

	static void KeyboardFuncWrapper(unsigned char key, int x, int y);
	void processNormalKeysDown(unsigned char key, int x, int y);

	static void KeyboardUpFuncWrapper(unsigned char key, int x, int y);
	void processNormalKeysUp(unsigned char key, int x, int y);

	static void SpecialFuncWrapper(int key, int x, int y);
	void processSpecialKeysDown(int key, int x, int y);

	static void SpecialUpFuncWrapper(int key, int x, int y);
	void processSpecialKeysUp(int key, int x, int y);

	static void PassiveMotionFuncWrapper(int xpos, int ypos);
	void processMouseInput(int xpos, int ypos);

	static void MouseFuncWrapper(int button, int state, int x, int y);
	void processMouseKeys(int button, int state, int x, int y);

	std::unordered_map<char, bool> keyStates;

public:
	static InputManager& get();
	void update();
};

