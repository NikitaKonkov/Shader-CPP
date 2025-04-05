#include "../include/shader_manager.h"


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

// Wrapper for ShaderToy fragment shaders
const char* shaderToyWrapper = R"(
    #version 330 core
    in vec2 fragCoord;
    out vec4 fragColor;
    
    uniform vec3 iResolution;
    uniform float iTime;
    uniform float iTimeDelta;
    uniform int iFrame;
    uniform vec4 iMouse;
    
    // Your ShaderToy code will be inserted here
    %s
    
    void main() {
        mainImage(fragColor, fragCoord * iResolution.xy);
    }
)";

// Function to render a frame with the ShaderToy shader
void renderShaderToyFrame(
        ShaderManager& shaderManager, 
        GLuint quadVAO, 
        int width, 
        int height, 
        float time, 
        float deltaTime, 
        int frame, 
        int mouseX, 
        int mouseY, 
        bool mouseDown
) {
    // Clear the screen
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Set up ShaderToy uniforms
    shaderManager.setupShaderToyUniforms(width, height, time, deltaTime, frame, mouseX, mouseY, mouseDown);
    
    // Draw the quad
    glBindVertexArray(quadVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}


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