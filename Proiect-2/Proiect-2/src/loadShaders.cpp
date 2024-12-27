//  Preluat si adaptat dupa http://www.opengl-tutorial.org/beginners-tutorials 


#include <GL/glew.h>
#include <GL/freeglut.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


void checkCompileErrors(unsigned int shader, std::string type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}


GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path, const char* tess_control_shader = nullptr, const char* tess_evaluation_shader = nullptr)
{
    GLuint ID;

    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::string tessControlCode;
    std::string tessEvaluationCode;

    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    std::ifstream tessControlShaderFile;
    std::ifstream tessEvaluationShaderFile;

    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    tessControlShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    tessEvaluationShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        // open files
        vShaderFile.open(vertex_file_path);
        fShaderFile.open(fragment_file_path);

        if (tess_control_shader)
            tessControlShaderFile.open(tess_control_shader);
        
        if (tess_evaluation_shader)
            tessEvaluationShaderFile.open(tess_evaluation_shader);

        // read file's buffer contents into streams
        std::stringstream vShaderStream, fShaderStream, tessControlStream, tessEvaluationStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        if (tess_control_shader)
            tessControlStream << tessControlShaderFile.rdbuf();
        
        if (tess_evaluation_shader)
            tessEvaluationStream << tessEvaluationShaderFile.rdbuf();

        // close file handlers
        vShaderFile.close();
        fShaderFile.close();

        if (tess_control_shader)
            tessControlShaderFile.close();

        if (tess_evaluation_shader)
            tessEvaluationShaderFile.close();

        // convert stream into string
        vertexCode   = vShaderStream.str();
        fragmentCode = fShaderStream.str();

        if (tess_control_shader)
            tessControlCode = tessControlStream.str();

        if (tess_evaluation_shader)
            tessEvaluationCode = tessEvaluationStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    const char* tessControlShaderCode = tessControlCode.size() != 0 ? tessControlCode.c_str() : nullptr;
    const char* tessEvaluationShaderCode = tessEvaluationCode.size() != 0 ? tessEvaluationCode.c_str() : nullptr;

    // 2. compile shaders

    // vertex shader
    unsigned int vertex;
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    // fragment Shader
    unsigned int fragment;
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    // Tessellation Control
    unsigned int tessellationControl;
    if (tessControlShaderCode)
    {
        tessellationControl = glCreateShader(GL_TESS_CONTROL_SHADER);
        glShaderSource(tessellationControl, 1, &tessControlShaderCode, NULL);
        glCompileShader(tessellationControl);
        checkCompileErrors(tessellationControl, "TESSELLATION_CONTROL");
    }

    // Tessellation Evaluation
    unsigned int tessellationEvaluation;
    if (tessEvaluationShaderCode)
    {
        tessellationEvaluation = glCreateShader(GL_TESS_EVALUATION_SHADER);
        glShaderSource(tessellationEvaluation, 1, &tessEvaluationShaderCode, NULL);
        glCompileShader(tessellationEvaluation);
        checkCompileErrors(tessellationEvaluation, "TESSELLATION_EVALUATION");
    }

    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);

    if (tessControlShaderCode)
        glAttachShader(ID, tessellationControl);

    if (tessEvaluationShaderCode)
        glAttachShader(ID, tessellationEvaluation);

    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");

    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return ID;
}

GLuint LoadComputeShader(const char* compute_file_path)
{
    GLuint ID;

    // 1. retrieve the compute source code from filePath
    std::string computeCode;
    std::ifstream computeShaderFile;

    // ensure ifstream objects can throw exceptions:
    computeShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // open files
        computeShaderFile.open(compute_file_path);

        std::stringstream computeShaderStream;
        // read file's buffer contents into streams
        computeShaderStream << computeShaderFile.rdbuf();

        // close file handlers
        computeShaderFile.close();

        // convert stream into string
        computeCode = computeShaderStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
    }
    const char* computeShaderCode = computeCode.c_str();

    // 2. compile shaders

    // compute shader
    unsigned int compute;
    compute = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(compute, 1, &computeShaderCode, NULL);
    glCompileShader(compute);
    checkCompileErrors(compute, "COMPUTE");

    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, compute);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");

    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(compute);

    return ID;
}