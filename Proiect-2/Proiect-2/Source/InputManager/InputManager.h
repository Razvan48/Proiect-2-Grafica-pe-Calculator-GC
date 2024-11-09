#pragma once

#include "../WindowManager/WindowManager.h"

class InputManager
{
private:
	InputManager(bool leftMouseButtonUp, bool leftMouseButtonHold, int currentMouseX, int currentMouseY);
	~InputManager();
	InputManager(const InputManager& other) = delete;
	InputManager& operator= (const InputManager& other) = delete;
	InputManager(const InputManager&& other) = delete;
	InputManager& operator= (const InputManager&& other) = delete;

	static void mouseFuncWrapper(int button, int state, int x, int y);
	void mouseFunc(int button, int state, int x, int y);
	static void motionFuncWrapper(int x, int y);
	void motionFunc(int x, int y);

	bool leftMouseButtonUp;
	bool leftMouseButtonHold;
	int currentMouseX;
	int currentMouseY;

public:
	static InputManager& get();
	void update();

	inline int getCurrentMouseX() const { return this->currentMouseX; }
	inline int getCurrentMouseY() const { return WindowManager::get().getWindowHeight() - this->currentMouseY; }
	inline bool getLeftMouseButtonUp() const { return this->leftMouseButtonUp; }
};

