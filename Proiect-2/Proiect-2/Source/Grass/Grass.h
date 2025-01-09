#pragma once

#include <GL/glew.h>

#include <glm/glm.hpp>

#include <vector>

struct Blade
{
	const glm::vec4 position;	// xyz: Position; w: orientation (in radius)
	const glm::vec4 v1;			// xyz: Bezier point; w: height
	const glm::vec4 v2;			// xyz: Physical model guide; w: width
	const glm::vec4 up;			// xyz: Up vector; w: stiffness coefficient

	// Constructor
	Blade(const glm::vec4& position, const glm::vec4& v1, const glm::vec4& v2, const glm::vec4& up);
};

class Grass
{
public:
	// Constructor
	Grass();

	// Destructor
	~Grass();

	void draw();
	void update();
	void createVAO(const std::vector<Blade>& blades);

	static void setupOpenGL();
	static Blade generateBlade(const glm::vec3& position);

	static float getThresholdWaterGrass() { return THRESH_WATER_GRASS; }

private:
	GLuint bladesCount;

	GLuint VAO;
	GLuint grassInputBuffer, grassOutputBuffer, grassIndirectBuffer;

	static GLuint grassComputeShader, grassShader;
	static const float windMagnitude;
	static const float windWaveLength;
	static const float windWavePeriod;

	static const float THRESH_WATER_GRASS;
};
