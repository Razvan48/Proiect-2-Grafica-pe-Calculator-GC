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