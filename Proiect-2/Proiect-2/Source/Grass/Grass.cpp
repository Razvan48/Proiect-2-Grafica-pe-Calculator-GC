#include "Grass.h"

#include <glm/ext/scalar_constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <loadShaders.h>

#include "../GlobalClock/GlobalClock.h"
#include "../Camera/Camera.h"

#include <iostream>
#include <random>

// Indirect drawing structure
struct NumBlades {
    std::uint32_t vertexCount = 5;
    std::uint32_t instanceCount = 1;
    std::uint32_t firstVertex = 0;
    std::uint32_t firstInstance = 0;
};

Blade::Blade(const glm::vec4& position, const glm::vec4& v1, const glm::vec4& v2, const glm::vec4& up)
    : position(position)
    , v1(v1)
    , v2(v2)
    , up(up)
{

}

GLuint Grass::grassComputeShader;
GLuint Grass::grassShader;

const float Grass::windMagnitude = 1.0f;
const float Grass::windWaveLength = 1.0f;
const float Grass::windWavePeriod = 1.0f;

const float Grass::THRESH_WATER_GRASS = 1.0f;

Grass::Grass()
{

}

Grass::~Grass()
{
    // Delete buffers
    glDeleteBuffers(1, &grassInputBuffer);
    glDeleteBuffers(1, &grassOutputBuffer);
    glDeleteBuffers(1, &grassIndirectBuffer);

    // Delete VAO
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &VAO);
}

void Grass::draw()
{
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    glBindVertexArray(VAO);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, grassInputBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, grassOutputBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, grassIndirectBuffer);

    glUseProgram(grassShader);

    glDrawArraysIndirect(GL_PATCHES, reinterpret_cast<void*>(0));
}

void Grass::update()
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, grassInputBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, grassOutputBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, grassIndirectBuffer);

    glUseProgram(grassComputeShader);

    glUniform1f(glGetUniformLocation(grassComputeShader, "current_time"), GlobalClock::get().getCurrentTime());
    glUniform1f(glGetUniformLocation(grassComputeShader, "delta_time"), GlobalClock::get().getDeltaTime());
    glUniform1f(glGetUniformLocation(grassComputeShader, "wind_magnitude"), windMagnitude);
    glUniform1f(glGetUniformLocation(grassComputeShader, "wind_wave_length"), windWaveLength);
    glUniform1f(glGetUniformLocation(grassComputeShader, "wind_wave_period"), windWavePeriod);

    glDispatchCompute(static_cast<GLuint>(bladesCount), 1, 1);
}

void Grass::createVAO(const std::vector<Blade>& blades)
{
    bladesCount = static_cast<GLuint>(blades.size());

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // grassInputBuffer
    glGenBuffers(1, &grassInputBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, grassInputBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, static_cast<GLsizei>(blades.size() * sizeof(Blade)), blades.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, grassInputBuffer);

    // grassOutputBuffer
    glGenBuffers(1, &grassOutputBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, grassOutputBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, static_cast<GLsizei>(blades.size() * sizeof(Blade)), nullptr, GL_STREAM_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, grassOutputBuffer);

    // grassIndirectBuffer
    NumBlades numBlades;
    glGenBuffers(1, &grassIndirectBuffer);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, grassIndirectBuffer);
    glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(NumBlades), &numBlades, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, grassOutputBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, grassIndirectBuffer);

    // position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Blade), reinterpret_cast<void*>(offsetof(Blade, position)));

    // v1 attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Blade), reinterpret_cast<void*>(offsetof(Blade, v1)));

    // v2 attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Blade), reinterpret_cast<void*>(offsetof(Blade, v2)));

    // dir attribute
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Blade), reinterpret_cast<void*>(offsetof(Blade, up)));
}

void Grass::setupOpenGL()
{
    glPatchParameteri(GL_PATCH_VERTICES, 1);

    // Compute Shader
    grassComputeShader = LoadComputeShader("shaders/grass/grass.comp");
    glUseProgram(grassComputeShader);

    // Vertex + Tessellation Control Shader + Tessellation Evaluation Shader + Fragment
    grassShader = LoadShaders("shaders/grass/grass.vert", "shaders/grass/grass.frag", "shaders/grass/grass.tesc", "shaders/grass/grass.tese");
}

Blade Grass::generateBlade(const glm::vec3& position)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> orientation_dis(0, glm::pi<float>());
    std::uniform_real_distribution<float> height_dis(0.6f, 1.2f);
    std::uniform_real_distribution<float> dis(-1, 1);

    const float x = position.x;
    const float y = position.y;
    const float z = position.z;
    const float blade_height = height_dis(gen);

    return Blade(
        glm::vec4(x, y, z, orientation_dis(gen)),
        glm::vec4(x, y + blade_height, z, blade_height),
        glm::vec4(x, y + blade_height, z, 0.1f),
        glm::vec4(0, y + blade_height, 0, 0.7f + dis(gen) * 0.3f)
    );
}
