#pragma once


class PerlinNoise2D
{
private:
	PerlinNoise2D();
	~PerlinNoise2D();
	PerlinNoise2D(const PerlinNoise2D& other) = delete;
	PerlinNoise2D& operator= (const PerlinNoise2D& other) = delete;
	PerlinNoise2D(const PerlinNoise2D&& other) = delete;
	PerlinNoise2D& operator= (const PerlinNoise2D&& other) = delete;

public:
	static PerlinNoise2D& get();
};