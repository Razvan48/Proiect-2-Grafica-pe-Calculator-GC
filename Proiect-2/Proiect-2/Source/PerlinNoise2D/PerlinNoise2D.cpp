#include "PerlinNoise2D.h"

#include "../RandomGenerator/RandomGenerator.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

PerlinNoise2D::PerlinNoise2D()
	: NUM_OCTAVES(3), GRID_MULTIPLIER(2), AMPLITUDE_DIMINISHMENT(0.5f)
	, INITIAL_GRID_SIZE(17), INITIAL_AMPLITUDE(5.0f), STARTING_VALUE(0.0f)
	, DEFAULT_INITIAL_HEIGHT(0.0f), HEIGHT_AMPLITUDE(1.0f)
	, MAX_COORDINATE_Y(666013)
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

std::pair<float, float> PerlinNoise2D::calculateGradient(int seed) const
{
	RandomGenerator::get().setSeed(seed);

	std::vector<std::pair<float, float>> possibleGradients = {
		{ 0.0f, 1.0f },
		{ 1.0f, 0.0f },
		{ 0.0f, -1.0f },
		{ -1.0f, 0.0f },

		{ 1.0f, 1.0f },
		{ 1.0f, -1.0f },
		{ -1.0f, 1.0f },
		{ -1.0f, -1.0f }
	};


	std::pair<float, float> chosenGradient = possibleGradients[RandomGenerator::get().randomUniformInt(0, (int)possibleGradients.size() - 1)];
	chosenGradient = std::pair<float, float>(RandomGenerator::get().randomUniformDouble(-1.0, 1.0), RandomGenerator::get().randomUniformDouble(-1.0, 1.0));

	// Normalization
	float length = glm::sqrt(chosenGradient.first * chosenGradient.first + chosenGradient.second * chosenGradient.second);

	if (length > 0.0f)
	{
		chosenGradient.first /= length;
		chosenGradient.second /= length;
	}

	return chosenGradient;
}

float PerlinNoise2D::calculateInitialHeight(int seed) const
{
	RandomGenerator::get().setSeed(seed);
	return this->DEFAULT_INITIAL_HEIGHT + RandomGenerator::get().randomUniformDouble(0, 1.0) * this->HEIGHT_AMPLITUDE;
}

float PerlinNoise2D::noisePerGrid(float x, float y, int currentGridSize) const
{
	int x0 = int(x / currentGridSize) * currentGridSize;
	int x1 = x0 + currentGridSize; // fara - 1 aici ca nu se intampla ce trebuie
	int y0 = int(y / currentGridSize) * currentGridSize;
	int y1 = y0 + currentGridSize; // fara - 1 aici ca nu se intampla ce trebuie

	int seed00 = this->hashCoordinates(x0, y0);
	int seed01 = this->hashCoordinates(x0, y1);
	int seed10 = this->hashCoordinates(x1, y0);
	int seed11 = this->hashCoordinates(x1, y1);

	std::pair<float, float> gradient00 = this->calculateGradient(seed00);
	std::pair<float, float> gradient01 = this->calculateGradient(seed01);
	std::pair<float, float> gradient10 = this->calculateGradient(seed10);
	std::pair<float, float> gradient11 = this->calculateGradient(seed11);

	std::pair<float, float> dist00 = { x - x0, y - y0 };
	std::pair<float, float> dist01 = { x - x0, y - y1 };
	std::pair<float, float> dist10 = { x - x1, y - y0 };
	std::pair<float, float> dist11 = { x - x1, y - y1 };

	std::pair<float, float> dist00Norm = { dist00.first / currentGridSize, dist00.second / currentGridSize };
	std::pair<float, float> dist01Norm = { dist01.first / currentGridSize, dist01.second / currentGridSize };
	std::pair<float, float> dist10Norm = { dist10.first / currentGridSize, dist10.second / currentGridSize };
	std::pair<float, float> dist11Norm = { dist11.first / currentGridSize, dist11.second / currentGridSize };

	float dot00 = gradient00.first * dist00Norm.first + gradient00.second * dist00Norm.second;
	float dot01 = gradient01.first * dist01Norm.first + gradient01.second * dist01Norm.second;
	float dot10 = gradient10.first * dist10Norm.first + gradient10.second * dist10Norm.second;
	float dot11 = gradient11.first * dist11Norm.first + gradient11.second * dist11Norm.second;

	float dot00Added = dot00 + this->calculateInitialHeight(seed00);
	float dot01Added = dot01 + this->calculateInitialHeight(seed01);
	float dot10Added = dot10 + this->calculateInitialHeight(seed10);
	float dot11Added = dot11 + this->calculateInitialHeight(seed11);

	float fade00 = this->fadeFunctionXY(1.0f - dist00Norm.first, 1.0f - dist00Norm.second);
	float fade01 = this->fadeFunctionXY(1.0f - dist01Norm.first, 1.0f + dist01Norm.second);
	float fade10 = this->fadeFunctionXY(1.0f + dist10Norm.first, 1.0f - dist10Norm.second);
	float fade11 = this->fadeFunctionXY(1.0f + dist11Norm.first, 1.0f + dist11Norm.second);

	float output = dot00Added * fade00 + dot01Added * fade01 + dot10Added * fade10 + dot11Added * fade11;
	return output;
}

float PerlinNoise2D::perlinNoise2D(float x, float y) const
{
	int currentGridSize = this->INITIAL_GRID_SIZE;
	float currentAmplitude = this->INITIAL_AMPLITUDE;
	float output = this->STARTING_VALUE;

	for (int i = 0; i < this->NUM_OCTAVES; ++i)
	{
		output += (currentAmplitude * this->noisePerGrid(x, y, currentGridSize));

		currentGridSize *= this->GRID_MULTIPLIER;
		currentAmplitude *= this->AMPLITUDE_DIMINISHMENT;
	}

	return output;
}