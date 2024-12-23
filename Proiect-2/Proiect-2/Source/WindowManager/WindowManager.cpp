#include "WindowManager.h"

const int WindowManager::WINDOW_WIDTH = 1400;
const int WindowManager::WINDOW_HEIGHT = 800;
const std::string WindowManager::WINDOW_TITLE = "Proiect-2";
const int WindowManager::WINDOW_POS_X = 100;
const int WindowManager::WINDOW_POS_Y = 100;

WindowManager::WindowManager()
    : windowWidth(WINDOW_WIDTH), windowHeight(WINDOW_HEIGHT)
{
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(WINDOW_POS_X, WINDOW_POS_Y);
    glutCreateWindow(WINDOW_TITLE.c_str());

    glewInit();

    glutReshapeFunc(WindowManager::reshapeFuncWrapper);

    glutSetCursor(GLUT_CURSOR_NONE);
}

WindowManager::~WindowManager()
{

}

void WindowManager::reshapeFuncWrapper(int width, int height)
{
    WindowManager::get().reshapeFunc(width, height);
}

void WindowManager::reshapeFunc(int width, int height)
{
    windowWidth = width;
    windowHeight = height;

    glutReshapeWindow(width, height);
    glViewport(0, 0, width, height);
}

WindowManager& WindowManager::get()
{
    static WindowManager instance;
    return instance;
}

