#include "PerlinNoise2D.h"

PerlinNoise2D::PerlinNoise2D()
{

}

PerlinNoise2D::~PerlinNoise2D()
{

}

PerlinNoise2D& PerlinNoise2D::get()
{
	static PerlinNoise2D instance;
	return instance;
}

float PerlinNoise2D::perlinNoise2D(float x, float y) const
{
	// TODO:
	return 0.0f;
}