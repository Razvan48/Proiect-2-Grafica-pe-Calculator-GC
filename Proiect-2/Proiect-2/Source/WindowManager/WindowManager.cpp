#include "WindowManager.h"

WindowManager::WindowManager()
    : WINDOW_WIDTH(1024), WINDOW_HEIGHT(768), WINDOW_TITLE("Proiect-2"), WINDOW_POS_X(100), WINDOW_POS_Y(100)
{
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(this->WINDOW_WIDTH, this->WINDOW_HEIGHT);
    glutInitWindowPosition(WINDOW_POS_X, WINDOW_POS_Y);
    glutCreateWindow(this->WINDOW_TITLE.c_str());

    glewInit();

    glutReshapeFunc(WindowManager::reshapeFuncWrapper);
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
    this->WINDOW_WIDTH = width;
    this->WINDOW_HEIGHT = height;

    glViewport(0, 0, this->WINDOW_WIDTH, this->WINDOW_HEIGHT);
}

WindowManager& WindowManager::get()
{
    static WindowManager instance;
    return instance;
}

