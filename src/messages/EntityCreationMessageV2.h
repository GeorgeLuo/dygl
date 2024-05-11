#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include "UniformData.h"

struct ShaderInfo
{
    std::string vertexShader;
    std::string fragmentShader;
};

struct Transform
{
    std::vector<float> position; // x, y, z
    std::vector<float> rotation; // quaternion x, y, z, w
    std::vector<float> scale;    // scale x, y, z
};

// struct UniformData
// {
//     std::unordered_map<std::string, std::vector<float>> floatVecUniforms;

//     // std::unordered_map<std::string, glm::vec2> vec2Uniforms;
//     // std::unordered_map<std::string, glm::vec3> vec3Uniforms;
//     // std::unordered_map<std::string, glm::vec4> vec4Uniforms;
//     // std::unordered_map<std::string, glm::mat3> mat3Uniforms;
//     // std::unordered_map<std::string, glm::mat4> mat4Uniforms;

//     // std::unordered_map<std::string, float> floatUniforms;
//     // std::unordered_map<std::string, int> intUniforms;
// };

struct VertexData
{
    std::vector<float> positions;
    std::vector<float> normals;
    std::vector<float> texCoords;
    std::vector<float> colors;
};

struct EntityCreationMessageV2
{
    int id;
    std::string shapeType;
    Transform transform;
    ShaderInfo shaders;
    UniformData uniforms;
    VertexData vertexData;

    EntityCreationMessageV2() {}

    EntityCreationMessageV2(int id, const std::string &shapeType, const Transform &transform,
                            const ShaderInfo &shaders, const UniformData &uniforms, const VertexData &vertexData)
        : id(id), shapeType(shapeType), transform(transform), shaders(shaders), uniforms(uniforms), vertexData(vertexData) {}
};
