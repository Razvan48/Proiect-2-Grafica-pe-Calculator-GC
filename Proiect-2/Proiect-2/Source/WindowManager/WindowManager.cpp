#include "WindowManager.h"

const int WindowManager::WINDOW_WIDTH = 1400;
const int WindowManager::WINDOW_HEIGHT = 800;

WindowManager::WindowManager()
    : WINDOW_TITLE("Proiect-2"), WINDOW_POS_X(100), WINDOW_POS_Y(100)
{
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(WINDOW_POS_X, WINDOW_POS_Y);
    glutCreateWindow(WINDOW_TITLE.c_str());

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
    glutReshapeWindow(WINDOW_WIDTH, WINDOW_HEIGHT);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
}

WindowManager& WindowManager::get()
{
    static WindowManager instance;
    return instance;
}

