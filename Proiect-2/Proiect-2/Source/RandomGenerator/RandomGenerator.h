#pragma once

#include <random>

class RandomGenerator
{
private:
	RandomGenerator();
	~RandomGenerator();
	RandomGenerator(const RandomGenerator& other) = delete;
	RandomGenerator& operator= (const RandomGenerator& other) = delete;
	RandomGenerator(const RandomGenerator&& other) = delete;
	RandomGenerator& operator= (const RandomGenerator&& other) = delete;

	int seed;
	std::mt19937 generator;

public:
	static RandomGenerator& get();

	void setSeed(int seed);

	double randomUniformDouble(double minimum, double maximum);
	int randomUniformInt(int minimum, int maximum);
};

