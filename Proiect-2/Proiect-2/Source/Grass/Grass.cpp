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

const float Grass::windMagnitude = 1.0f;
const float Grass::windWaveLength = 1.0f;
const float Grass::windWavePeriod = 1.0f;

Grass::Grass()
{
    glPatchParameteri(GL_PATCH_VERTICES, 1);

    createVAO();
    loadShadersGrass();
}

Grass::~Grass()
{
    // Delete VAO + VBO
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &VAO);

    // Delete shaders
    glDeleteProgram(grassShader);
    glDeleteProgram(grassComputeShader);
}

void Grass::draw()
{
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    glBindVertexArray(VAO);

    glUseProgram(grassShader);

    glDrawArraysIndirect(GL_PATCHES, reinterpret_cast<void*>(0));
}

void Grass::update()
{
    glUseProgram(grassComputeShader);

    // TODO: verifica daca asta e ok
    glUniform1f(glGetUniformLocation(grassComputeShader, "current_time"), GlobalClock::get().getCurrentTime());
    // grass_compute_shader_.setFloat("current_time", static_cast<float>(glfwGetTime()));

    // TODO: verifica daca asta e ok
    glUniform1f(glGetUniformLocation(grassComputeShader, "delta_time"), GlobalClock::get().getDeltaTime());
    // grass_compute_shader_.setFloat("delta_time", delta_time.count() / 1e3f);

    glUniform1f(glGetUniformLocation(grassComputeShader, "wind_magnitude"), windMagnitude);
    glUniform1f(glGetUniformLocation(grassComputeShader, "wind_wave_length"), windWaveLength);
    glUniform1f(glGetUniformLocation(grassComputeShader, "wind_wave_period"), windWavePeriod);

    glDispatchCompute(static_cast<GLuint>(bladesCount), 1, 1);
}

std::vector<Blade> Grass::generateBlades()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> orientation_dis(0, glm::pi<float>());
    std::uniform_real_distribution<float> height_dis(0.6f, 1.2f);
    std::uniform_real_distribution<float> dis(-1, 1);

    std::vector<Blade> blades;
    for (int i = -200; i < 200; ++i)
    {
        for (int j = -200; j < 200; ++j)
        {
            const auto x = static_cast<float>(i) / 10 - 1 + dis(gen) * 0.1f;
            const auto y = static_cast<float>(j) / 10 - 1 + dis(gen) * 0.1f;
            const auto blade_height = height_dis(gen);

            blades.emplace_back(Blade(
                glm::vec4(x, 0, y, orientation_dis(gen)),
                glm::vec4(x, blade_height, y, blade_height),
                glm::vec4(x, blade_height, y, 0.1f),
                glm::vec4(0, blade_height, 0, 0.7f + dis(gen) * 0.3f)
            ));
        }
    }

    return blades;
}

void Grass::createVAO()
{
    const std::vector<Blade> blades = generateBlades();
    bladesCount = static_cast<GLuint>(blades.size());

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    GLuint grassInputBuffer = 0;
    glGenBuffers(1, &grassInputBuffer);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, grassInputBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, static_cast<GLsizei>(blades.size() * sizeof(Blade)), blades.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, grassInputBuffer);

    unsigned int grassOutputBuffer = 0;
    glGenBuffers(1, &grassOutputBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, grassOutputBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, static_cast<GLsizei>(blades.size() * sizeof(Blade)), nullptr, GL_STREAM_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, grassOutputBuffer);

    NumBlades numBlades;
    unsigned int grassIndirectBuffer = 0;
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

void Grass::loadShadersGrass()
{
    // Compute Shader
    grassComputeShader = LoadComputeShader("shaders/grass/grass.comp");
    glUseProgram(grassComputeShader);

    // Vertex + Tessellation Control Shader + Tessellation Evaluation Shader + Fragment
    grassShader = LoadShaders("shaders/grass/grass.vert", "shaders/grass/grass.frag", "shaders/grass/grass.tesc", "shaders/grass/grass.tese");
}
