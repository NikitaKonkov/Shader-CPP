#include "shadertoy_utils.h"

// Default vertex shader for ShaderToy-style rendering
const char* defaultVertexShader = R"(
    #version 330 core
    layout (location = 0) in vec3 position;
    layout (location = 1) in vec2 texCoord;
    
    out vec2 fragCoord;
    
    void main()
    {
        gl_Position = vec4(position, 1.0);
        fragCoord = texCoord;
    }
)";

// Create a ShaderToy-compatible fragment shader
std::string createShaderToyFragmentShader(const std::string& shaderToyCode) {
    std::string wrapper = R"(
        #version 330 core
        in vec2 fragCoord;
        out vec4 fragColor;
        
        uniform vec3 iResolution;
        uniform float iTime;
        uniform float iTimeDelta;
        uniform int iFrame;
        uniform vec4 iMouse;
        
        // ShaderToy code
        )";
        
    wrapper += shaderToyCode;
    
    wrapper += R"(
        
        void main() {
            mainImage(fragColor, fragCoord * iResolution.xy);
        }
    )";
    
    return wrapper;
}