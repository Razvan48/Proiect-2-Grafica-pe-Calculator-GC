#pragma once

#include "../Camera//Camera.h"
#include "../MapChunk/MapChunk.h"

#include <vector>
#include <mutex>

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
	std::mutex mapChunksMutex;

	GLuint programId;

	void addMapChunk(MapChunk& mapChunk);

public:
	static Map& get();

	void draw();
	void update();

	inline GLuint getProgramId() const { return programId; }
};