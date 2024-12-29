#include "RandomGenerator.h"

RandomGenerator::RandomGenerator()
    : seed(0)
{
	this->generator = std::mt19937(std::random_device()());
	this->generator.seed(this->seed);
}

RandomGenerator::RandomGenerator(int seed)
	: seed(seed)
{
	this->generator = std::mt19937(std::random_device()());
	this->generator.seed(this->seed);
}

RandomGenerator::~RandomGenerator()
{

}

void RandomGenerator::setSeed(int seed)
{
	this->seed = seed;
	this->generator.seed(this->seed);
}

double RandomGenerator::randomUniformDouble(double minimum, double maximum)
{
    std::uniform_real_distribution<double> distribution(minimum, maximum);
    return distribution(this->generator);
}

int RandomGenerator::randomUniformInt(int minimum, int maximum)
{
    std::uniform_int_distribution<int> distribution(minimum, maximum);
    return distribution(this->generator);
}



