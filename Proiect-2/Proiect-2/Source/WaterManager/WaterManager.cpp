#include "WaterManager.h"

#include "../WindowManager/WindowManager.h"
#include <loadShaders.h>


WaterManager::WaterManager()
	: fbos(WindowManager::get().getWidth(), WindowManager::get().getHeight())
{
	this->programId = LoadShaders("shaders/water/water.vert", "shaders/water/water.frag");
}

WaterManager::~WaterManager()
{
	glDeleteProgram(this->programId);
}

WaterManager& WaterManager::get()
{
	static WaterManager instance;
	return instance;
}

void WaterManager::draw()
{
	for (auto& i : WaterChunks)
		i.draw();
}

void WaterManager::setWaterHeight(float WaterHeight_)
{
	this->WaterHeight = WaterHeight_;

	for (size_t i = 0;i < WaterChunks.size(); ++i)
	{
		WaterChunks[i].setHeight(this->WaterHeight);
	}
}