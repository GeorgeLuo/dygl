#pragma once
#include <string>
#include <unordered_map>
#include <vector>

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

struct UniformData
{
    std::unordered_map<std::string, std::vector<float>> floatUniforms;
    // Add other uniform types as necessary
};

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
