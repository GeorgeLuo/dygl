#include "ShaderManager.h"
#include <fstream>
#include <iostream>
#include <sstream>

ShaderManager::ShaderManager() {}

ShaderManager::~ShaderManager() {
    for (auto& shader : compiledShaders) {
        glDeleteProgram(shader.first);
        glDeleteShader(shader.second.first); // Vertex Shader
        glDeleteShader(shader.second.second); // Fragment Shader
    }
}

GLuint ShaderManager::CompileAndLinkShaders(const std::string& vertexCode, const std::string& fragmentCode) {
    GLuint vertexShader = CompileShader(vertexCode, GL_VERTEX_SHADER);
    GLuint fragmentShader = CompileShader(fragmentCode, GL_FRAGMENT_SHADER);

    GLuint programID = glCreateProgram();
    glAttachShader(programID, vertexShader);
    glAttachShader(programID, fragmentShader);
    glLinkProgram(programID);

    // Check for linking errors
    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(programID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // Shaders are linked into our program and no longer necessary
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    compiledShaders[programID] = std::make_pair(vertexShader, fragmentShader);

    return programID;
}

GLuint ShaderManager::LoadShaderProgram(const std::string& vertexPath, const std::string& fragmentPath) {
    std::string vertexCode = ReadShaderFromFile(vertexPath);
    std::string fragmentCode = ReadShaderFromFile(fragmentPath);
    // Generate hash keys for both shaders
    std::hash<std::string> hasher;
    std::string vertexHash = std::to_string(hasher(vertexCode));
    std::string fragmentHash = std::to_string(hasher(fragmentCode));
    std::string combinedHash = vertexHash + fragmentHash;
    // Check if we already have a compiled program for this combination
    auto it = shadersById.find(combinedHash);
    if(it != shadersById.end()) {
        // Program already compiled, return existing GLuint
        return it->second;
    }
    // No existing program, compile, link, and store as before
    GLuint programID = CompileAndLinkShaders(vertexCode, fragmentCode);
    shadersById[combinedHash] = programID;
    return programID;
}


void ShaderManager::UseShader(GLuint programID) {
    glUseProgram(programID);
}

void ShaderManager::SetUniform1i(GLuint programID, const std::string& name, int value) {
    glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
}

void ShaderManager::SetUniform1f(GLuint programID, const std::string& name, float value) {
    glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
}

void ShaderManager::SetUniform3f(GLuint programID, const std::string& name, float v1, float v2, float v3) {
    glUniform3f(glGetUniformLocation(programID, name.c_str()), v1, v2, v3);
}

void ShaderManager::SetUniform4fv(GLuint programID, const std::string &name, const glm::vec4 &value) {
    glUniform4f(glGetUniformLocation(programID, name.c_str()), value[0], value[1], value[2], value[3]);
}

void ShaderManager::SetUniformMatrix4fv(GLuint programID, const std::string& name, const GLfloat* value) {
    glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, value);
}

GLuint ShaderManager::CompileShader(const std::string& source, GLenum type) {
    GLuint shader = glCreateShader(type);
    const GLchar* shaderSource = source.c_str();
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);

    // Check for compile errors
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::string shaderType = (type == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT";
        std::cout << "ERROR::SHADER::" << shaderType << "::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    return shader;
}

std::string ShaderManager::ReadShaderFromFile(const std::string& filePath) {
    std::ifstream shaderFile(filePath);
    std::stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();
    shaderFile.close();
    return shaderStream.str();
}