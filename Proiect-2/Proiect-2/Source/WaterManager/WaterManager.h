#pragma once

#include "../Water/Water.h"

#include <vector>

class WaterManager
{
private:
	WaterManager();
	~WaterManager();
	WaterManager(const WaterManager& other) = delete;
	WaterManager& operator= (const WaterManager& other) = delete;
	WaterManager(const WaterManager&& other) = delete;
	WaterManager& operator= (const WaterManager&& other) = delete;

	GLuint programId;
	WaterFrameBuffers fbos;

	std::vector<Water> WaterChunks;

	float WaterHeight = 5.0;

	friend class Map;


public:
	static WaterManager& get();

	inline GLuint getProgramId() const { return programId; }
	inline WaterFrameBuffers& getfbos() { return fbos; }
	inline float getWaterHeight() const { return WaterHeight; };

	void setWaterHeight(float WaterHeight_);

	void draw();

	//inline void push_back(const Water& waterChunk) { WaterChunks.push_back(waterChunk); }
	//inline void pop_back() { WaterChunks.pop_back(); }
};