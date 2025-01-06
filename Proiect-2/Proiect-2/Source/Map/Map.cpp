#include "Map.h"

#include "loadShaders.h"
#include "../GlobalClock/GlobalClock.h"

#include <thread>

Map::Map()
	: NUM_CHUNKS_AHEAD(9)
	, lastTimeLoadedOpenGL(0.0f)
	, TIME_BETWEEN_OPENGL_LOADS(1.0f / 45.0f)
{
	this->mapChunks.reserve((this->NUM_CHUNKS_AHEAD * 2 + 1) * (this->NUM_CHUNKS_AHEAD * 2 + 1));

	this->programId = LoadShaders("shaders/chunkMap/chunkMap.vert", "shaders/chunkMap/chunkMap.frag");
}

Map::~Map()
{
	glDeleteProgram(this->programId);

	this->mapChunksMutex.lock();
	this->mapChunks.clear();
	this->mapChunksMutex.unlock();

	this->chunksAlreadyBeingLoadedMutex.lock();
	this->chunksAlreadyBeingLoaded.clear();
	this->chunksAlreadyBeingLoadedMutex.unlock();
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
		if (this->mapChunks[i].getOpenGLSetupDone())
			this->mapChunks[i].draw();
	this->mapChunksMutex.unlock();



	this->mapChunksMutex.lock();
	for (int i = 0; i < this->mapChunks.size(); ++i)
	{
		if (!this->mapChunks[i].getOpenGLSetupDone())
		{
			if (GlobalClock::get().getCurrentTime() - this->lastTimeLoadedOpenGL > this->TIME_BETWEEN_OPENGL_LOADS)
			{
				this->mapChunks[i].setupOpenGL();
				this->lastTimeLoadedOpenGL = GlobalClock::get().getCurrentTime();
			}
		}
	}
	this->mapChunksMutex.unlock();
}

void Map::update()
{
	MapChunk::commonUpdate();

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
			if (!chunksAlreadyLoaded[i + this->NUM_CHUNKS_AHEAD][j + this->NUM_CHUNKS_AHEAD] && cameraChunkX + i >= 0 && cameraChunkY + j >= 0
				&& !this->isChunkBeingLoaded(cameraChunkX + i, cameraChunkY + j))
			{
				this->addChunkToBeingLoaded(cameraChunkX + i, cameraChunkY + j);

				std::thread mapChunkThread([this, cameraChunkX, cameraChunkY, i, j]()
					{
						MapChunk mapChunk(cameraChunkX + i, cameraChunkY + j);
						this->addMapChunk(mapChunk);
						this->removeChunkFromBeingLoaded(cameraChunkX + i, cameraChunkY + j);
					});
				mapChunkThread.detach();
			}
		}
	}
}

void Map::addMapChunk(MapChunk& mapChunk)
{
	this->mapChunksMutex.lock();
	this->mapChunks.push_back(std::move(mapChunk));
	this->mapChunksMutex.unlock();
}

void Map::addChunkToBeingLoaded(int x, int y)
{
	this->chunksAlreadyBeingLoadedMutex.lock();
	this->chunksAlreadyBeingLoaded.insert(std::make_pair(x, y));
	this->chunksAlreadyBeingLoadedMutex.unlock();
}

void Map::removeChunkFromBeingLoaded(int x, int y)
{
	this->chunksAlreadyBeingLoadedMutex.lock();
	this->chunksAlreadyBeingLoaded.erase(std::make_pair(x, y));
	this->chunksAlreadyBeingLoadedMutex.unlock();
}

bool Map::isChunkBeingLoaded(int x, int y)
{
	this->chunksAlreadyBeingLoadedMutex.lock();
	bool isChunkInSet = (this->chunksAlreadyBeingLoaded.find(std::make_pair(x, y)) != this->chunksAlreadyBeingLoaded.end());
	this->chunksAlreadyBeingLoadedMutex.unlock();

	return isChunkInSet;
}

glm::vec2 Map::getTopLeftCornerOfVisibleMap() const
{
	int cameraChunkX = MapChunk::calculateChunkX(Camera::get().getPosition().x);
	int cameraChunkY = MapChunk::calculateChunkY(Camera::get().getPosition().z);

	float x = glm::max(0.0f, 1.0f * (cameraChunkX - this->NUM_CHUNKS_AHEAD) * MapChunk::CHUNK_SIZE);
	float y = glm::max(0.0f, 1.0f * (cameraChunkY - this->NUM_CHUNKS_AHEAD) * MapChunk::CHUNK_SIZE);

	return glm::vec2(x, y);
}

glm::vec2 Map::getBottomRightCornerOfVisibleMap() const
{
	int cameraChunkX = MapChunk::calculateChunkX(Camera::get().getPosition().x);
	int cameraChunkY = MapChunk::calculateChunkY(Camera::get().getPosition().z);

	float x = glm::max(0.0f, 1.0f * (cameraChunkX + this->NUM_CHUNKS_AHEAD) * MapChunk::CHUNK_SIZE + MapChunk::CHUNK_SIZE);
	float y = glm::max(0.0f, 1.0f * (cameraChunkY + this->NUM_CHUNKS_AHEAD) * MapChunk::CHUNK_SIZE + MapChunk::CHUNK_SIZE);

	return glm::vec2(x, y);
}

