#include "Map.h"

#include "loadShaders.h"


Map::Map()
	: NUM_CHUNKS_AHEAD(5)
{
	this->mapChunks.reserve((this->NUM_CHUNKS_AHEAD * 2 + 1) * (this->NUM_CHUNKS_AHEAD * 2 + 1));

	int cameraChunkX = MapChunk::calculateChunkX(Camera::get().getPosition().x);
	int cameraChunkY = MapChunk::calculateChunkY(Camera::get().getPosition().z);

	for (int i = -this->NUM_CHUNKS_AHEAD; i <= this->NUM_CHUNKS_AHEAD; ++i)
		for (int j = -this->NUM_CHUNKS_AHEAD; j <= this->NUM_CHUNKS_AHEAD; ++j)
			if (cameraChunkX + i >= 0 && cameraChunkY + j >= 0)
				this->mapChunks.emplace_back(cameraChunkX + i, cameraChunkY + j);

	this->programId = LoadShaders("shaders/chunkMap/chunkMap.vert", "shaders/chunkMap/chunkMap.frag");
}

Map::~Map()
{

}

Map& Map::get()
{
	static Map instance;
	return instance;
}

void Map::draw()
{
	for (int i = 0; i < this->mapChunks.size(); ++i)
		this->mapChunks[i].draw();
}

void Map::update()
{
	for (int i = 0; i < this->mapChunks.size(); ++i)
		this->mapChunks[i].update();



	std::vector<std::vector<bool>> chunksAlreadyLoaded;
	chunksAlreadyLoaded.resize(this->NUM_CHUNKS_AHEAD * 2 + 1);
	for (int i = 0; i < chunksAlreadyLoaded.size(); ++i)
		chunksAlreadyLoaded[i].resize(this->NUM_CHUNKS_AHEAD * 2 + 1, false);

	int cameraChunkX = MapChunk::calculateChunkX(Camera::get().getPosition().x);
	int cameraChunkY = MapChunk::calculateChunkY(Camera::get().getPosition().z);

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

	for (int i = -this->NUM_CHUNKS_AHEAD; i <= this->NUM_CHUNKS_AHEAD; ++i)
		for (int j = -this->NUM_CHUNKS_AHEAD; j <= this->NUM_CHUNKS_AHEAD; ++j)
			if (!chunksAlreadyLoaded[i + this->NUM_CHUNKS_AHEAD][j + this->NUM_CHUNKS_AHEAD] && cameraChunkX + i >= 0 && cameraChunkY + j >= 0)
				this->mapChunks.emplace_back(cameraChunkX + i, cameraChunkY + j);
}