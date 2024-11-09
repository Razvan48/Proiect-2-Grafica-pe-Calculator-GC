#include "GlobalClock.h"

#include <GL/glew.h>
#include <GL/freeglut.h>

GlobalClock::GlobalClock(float currentTime, float lastTime, float deltaTime)
	: currentTime(currentTime), lastTime(lastTime), deltaTime(deltaTime)
{

}

GlobalClock::~GlobalClock()
{

}

GlobalClock& GlobalClock::get()
{
	static GlobalClock instance(0.0f, 0.0f, 0.0f);
	return instance;
}

void GlobalClock::update()
{
	this->lastTime = this->currentTime;
	this->currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f; // in secunde, deoarece glutGet ne intoarce numarul de milisecunde
	this->deltaTime = this->currentTime - this->lastTime;
}

