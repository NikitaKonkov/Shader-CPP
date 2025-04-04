#ifndef SHADERTOY_UTILS_H
#define SHADERTOY_UTILS_H

#include <string>

// Default vertex shader for ShaderToy-style rendering
extern const char* defaultVertexShader;

// Create a ShaderToy-compatible fragment shader
std::string createShaderToyFragmentShader(const std::string& shaderToyCode);

#endif // SHADERTOY_UTILS_H