#pragma once
#include <unordered_map>
#include <vector>

struct UniformData
{
    std::unordered_map<std::string, std::vector<float>> floatVecUniforms;

    // std::unordered_map<std::string, glm::vec2> vec2Uniforms;
    // std::unordered_map<std::string, glm::vec3> vec3Uniforms;
    // std::unordered_map<std::string, glm::vec4> vec4Uniforms;
    // std::unordered_map<std::string, glm::mat3> mat3Uniforms;
    std::unordered_map<std::string, glm::mat4> mat4Uniforms;

    // std::unordered_map<std::string, float> floatUniforms;
    std::unordered_map<std::string, std::vector<int>> intUniforms;
};