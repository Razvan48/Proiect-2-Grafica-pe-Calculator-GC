#pragma once

#include <utility>

class PerlinNoise2D
{
private:
	PerlinNoise2D();
	~PerlinNoise2D();
	PerlinNoise2D(const PerlinNoise2D& other) = delete;
	PerlinNoise2D& operator= (const PerlinNoise2D& other) = delete;
	PerlinNoise2D(const PerlinNoise2D&& other) = delete;
	PerlinNoise2D& operator= (const PerlinNoise2D&& other) = delete;

	const int NUM_OCTAVES;

	const int GRID_MULTIPLIER;
	const float AMPLITUDE_DIMINISHMENT;

	const int INITIAL_GRID_SIZE;
	const float INITIAL_AMPLITUDE;
	const float STARTING_VALUE;

	const float DEFAULT_INITIAL_HEIGHT;
	const float HEIGHT_AMPLITUDE;

	const int MAX_COORDINATE_Y;

	inline int hashCoordinates(int x, int y) const { return x * this->MAX_COORDINATE_Y + y; }
	std::pair<float, float> calculateGradient(int seed) const;
	float calculateInitialHeight(int seed) const;
	inline float fadeFunction(float t) const { return 6.0f * t * t * t * t * t - 15.0f * t * t * t * t + 10.0f * t * t * t; }
	inline float fadeFunctionXY(float x, float y) const { return this->fadeFunction(x) * this->fadeFunction(y); }

	float noisePerGrid(int x, int y, int currentGridSize) const;

public:
	static PerlinNoise2D& get();

	float perlinNoise2D(float x, float y) const;
};