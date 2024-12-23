#pragma once

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <string>

class WindowManager
{
private:
	// Constructor
	WindowManager();

	// Destructor
	~WindowManager();

	// Not copyable
	WindowManager(const WindowManager& other) = delete;
	WindowManager& operator= (const WindowManager& other) = delete;

	// Not moveable
	WindowManager(const WindowManager&& other) = delete;
	WindowManager& operator= (const WindowManager&& other) = delete;

	static void reshapeFuncWrapper(int width, int height);
	void reshapeFunc(int width, int height);

	static const int WINDOW_WIDTH;
	static const int WINDOW_HEIGHT;
	static const std::string WINDOW_TITLE;
	static const int WINDOW_POS_X;
	static const int WINDOW_POS_Y;

	int windowWidth, windowHeight;

public:
	static WindowManager& get();

	inline int getWidth() { return windowWidth; }
	inline int getHeight() { return windowHeight; }
};
