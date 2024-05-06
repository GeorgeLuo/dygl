#pragma once
#include <glad.h> // Include GLAD
#include <string>
#include <unordered_map>
#include <glm.hpp>
#include <vector>
#include "GeometryComponent.h"
#include "Vertex.h"

class ShaderManager
{
public:
    ShaderManager();
    ~ShaderManager();

    // Load, compile, and link shaders from files
    GLuint LoadShaderProgram(const std::string &vertexPath, const std::string &fragmentPath);

    // Load, compile, and link shaders from files
    GLuint CompileAndLinkShaders(const std::string &vertexPath, const std::string &fragmentPath);

    // Use (activate) the shader program
    void UseShader(GLuint programID);

    // ShaderManager.h, corrected method signatures with programID
    void SetUniform1i(GLuint programID, const std::string &name, int value);
    void SetUniform1f(GLuint programID, const std::string &name, float value);
    void SetUniform3f(GLuint programID, const std::string &name, float v1, float v2, float v3);
    void SetUniform4fv(GLuint programID, const std::string &name, const glm::vec4 &value);
    void SetUniformMatrix4fv(GLuint programID, const std::string &name, const GLfloat *value); // Notice change in last parameter type to match cpp file

    // Initializes VAO and VBO for a given set of vertices and stores them for reuse
    void SetupGeometry(const std::vector<Vertex> &vertices, unsigned int &VAO, unsigned int &VBO);

    // Cleanup resources
    void Cleanup();

private:
    GLuint currentProgramID;                                               // Keep track of the current in-use shader program
    std::unordered_map<GLuint, std::pair<GLuint, GLuint>> compiledShaders; // Stores compiled shaders for cleanup
    std::unordered_map<std::string, GLuint> shadersById;

    // Methods to compile individual shaders
    GLuint CompileShader(const std::string &source, GLenum type);

    // Utility function to read shader source code from file
    std::string ReadShaderFromFile(const std::string &filePath);
};