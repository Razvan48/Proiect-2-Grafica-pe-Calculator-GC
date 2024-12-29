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

	float lastTimeLoadedOpenGL;
	const float TIME_BETWEEN_OPENGL_LOADS;

public:
	static Map& get();

	void draw();
	void update();

	inline GLuint getProgramId() const { return programId; }
};