#pragma once

#include "../Camera//Camera.h"
#include "../MapChunk/MapChunk.h"

#include <vector>

class Map
{
private:
	Map();
	~Map();
	Map(const Map& other) = delete;
	Map& operator= (const Map& other) = delete;
	Map(const Map&& other) = delete;
	Map& operator= (const Map&& other) = delete;

	const int NUM_CHUNKS_AHEAD;
	std::vector<MapChunk> mapChunks;

public:
	static Map& get();

	void draw();
	void update();
};