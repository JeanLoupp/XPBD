#ifndef COMPUTE_SHADER_HPP
#define COMPUTE_SHADER_HPP

#include "ShaderProgram.hpp"

class ComputeShader : public ShaderProgram {
public:
    ComputeShader(const std::string &computeShaderPath);
};

#endif // COMPUTE_SHADER_HPP