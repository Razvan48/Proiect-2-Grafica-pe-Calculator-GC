#pragma once

#include <random>

class RandomGenerator
{
private:

	int seed;
	std::mt19937 generator;

public:
	RandomGenerator();
	RandomGenerator(int seed);
	~RandomGenerator();

	void setSeed(int seed);

	double randomUniformDouble(double minimum, double maximum);
	int randomUniformInt(int minimum, int maximum);
};

