#include "Map.h"

#include "loadShaders.h"

#include <thread>


Map::Map()
	: NUM_CHUNKS_AHEAD(9)
{
	this->mapChunks.reserve((this->NUM_CHUNKS_AHEAD * 2 + 1) * (this->NUM_CHUNKS_AHEAD * 2 + 1));

	this->programId = LoadShaders("shaders/chunkMap/chunkMap.vert", "shaders/chunkMap/chunkMap.frag");
}

Map::~Map()
{
	glDeleteProgram(this->programId);
}

Map& Map::get()
{
	static Map instance;
	return instance;
}

void Map::draw()
{
	this->mapChunksMutex.lock();
	for (int i = 0; i < this->mapChunks.size(); ++i)
	{
		if (!this->mapChunks[i].getOpenGLSetupDone())
		{
			this->mapChunks[i].setupOpenGL();
			break;
		}
		this->mapChunks[i].draw();
	}
	this->mapChunksMutex.unlock();
}

void Map::update()
{
	this->mapChunksMutex.lock();
	for (int i = 0; i < this->mapChunks.size(); ++i)
		this->mapChunks[i].update();
	this->mapChunksMutex.unlock();



	std::vector<std::vector<bool>> chunksAlreadyLoaded;
	chunksAlreadyLoaded.resize(this->NUM_CHUNKS_AHEAD * 2 + 1);
	for (int i = 0; i < chunksAlreadyLoaded.size(); ++i)
		chunksAlreadyLoaded[i].resize(this->NUM_CHUNKS_AHEAD * 2 + 1, false);

	int cameraChunkX = MapChunk::calculateChunkX(Camera::get().getPosition().x);
	int cameraChunkY = MapChunk::calculateChunkY(Camera::get().getPosition().z);


	this->mapChunksMutex.lock();
	for (int i = 0; i < this->mapChunks.size(); ++i)
	{
		int chunkX = this->mapChunks[i].getX();
		int chunkY = this->mapChunks[i].getY();

		if (abs(chunkX - cameraChunkX) > this->NUM_CHUNKS_AHEAD || abs(chunkY - cameraChunkY) > this->NUM_CHUNKS_AHEAD)
		{
			std::swap(this->mapChunks[i], this->mapChunks.back());
			this->mapChunks.pop_back();
			--i;
		}
		else
			chunksAlreadyLoaded[chunkX - cameraChunkX + this->NUM_CHUNKS_AHEAD][chunkY - cameraChunkY + this->NUM_CHUNKS_AHEAD] = true;
	}
	this->mapChunksMutex.unlock();

	for (int i = -this->NUM_CHUNKS_AHEAD; i <= this->NUM_CHUNKS_AHEAD; ++i)
	{
		for (int j = -this->NUM_CHUNKS_AHEAD; j <= this->NUM_CHUNKS_AHEAD; ++j)
		{
			if (!chunksAlreadyLoaded[i + this->NUM_CHUNKS_AHEAD][j + this->NUM_CHUNKS_AHEAD] && cameraChunkX + i >= 0 && cameraChunkY + j >= 0)
			{
				std::thread mapChunkThread([this, cameraChunkX, cameraChunkY, i, j]()
					{
						MapChunk mapChunk(cameraChunkX + i, cameraChunkY + j);
						this->addMapChunk(mapChunk);
					});
				mapChunkThread.detach();
			}
		}
	}
}

void Map::addMapChunk(MapChunk& mapChunk)
{
	this->mapChunksMutex.lock();
	bool mapChunkAlreadyLoaded = false;
	for (int i = 0; i < this->mapChunks.size(); ++i)
	{
		if (this->mapChunks[i].getX() == mapChunk.getX() && this->mapChunks[i].getY() == mapChunk.getY())
		{
			mapChunkAlreadyLoaded = true;
			break;
		}
	}
	if (!mapChunkAlreadyLoaded)
		this->mapChunks.push_back(std::move(mapChunk));
	this->mapChunksMutex.unlock();
}

