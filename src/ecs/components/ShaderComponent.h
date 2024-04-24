#pragma once
#include <string>
#include <iostream>

struct ShaderComponent {
    // filenames of shaders
    std::string vertexShader;
    std::string fragmentShader;

    ShaderComponent() = default;
    ShaderComponent(std::string vertexShader, std::string fragmentShader): vertexShader(vertexShader), fragmentShader(fragmentShader) {};
};