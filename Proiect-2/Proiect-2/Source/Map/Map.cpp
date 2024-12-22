#include "Map.h"


Map::Map()
	: NUM_CHUNKS_AHEAD(2)
{
	this->mapChunks.reserve((NUM_CHUNKS_AHEAD * 2 + 1) * (NUM_CHUNKS_AHEAD * 2 + 1));

	// TODO:

	/*
	int currentChunkX = MapChunk::CalculateChunkX(Camera::get().);

	for (int i = currentChunkX -NUM_CHUNKS_AHEAD; i <= NUM_CHUNKS_AHEAD; i++)
	{
		for (int j = -NUM_CHUNKS_AHEAD; j <= NUM_CHUNKS_AHEAD; j++)
		{
			this->mapChunks.push_back(MapChunk(i, j));
		}
	}
	*/
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
	// TODO:
}