#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#include "includes.h"


// Default vertex shader for ShaderToy-style rendering
extern const char* defaultVertexShader;

// Create a ShaderToy-compatible fragment shader
std::string createShaderToyFragmentShader(const std::string& shaderToyCode);



class ShaderManager {
public:
    // Constructor and destructor
    ShaderManager();
    ~ShaderManager();
    
    // Load shaders from strings
    bool loadFromStrings(const std::string& vertexSource, const std::string& fragmentSource);
    
    // Use the shader program
    void use();
    
    // Set uniform values
    void setFloat(const std::string& name, float value);
    void setInt(const std::string& name, int value);
    void setVec2(const std::string& name, float x, float y);
    void setVec3(const std::string& name, float x, float y, float z);
    void setVec4(const std::string& name, float x, float y, float z, float w);
    
    // ShaderToy specific functions
    void setupShaderToyUniforms(int windowWidth, int windowHeight, float time, float deltaTime, int frame, int mouseX, int mouseY, bool mouseDown);
    
    // Get the program ID
    GLuint getProgramID() const { return programID; }

private:
    GLuint programID;
    bool checkCompileErrors(GLuint shader, const std::string& type);
    bool checkLinkErrors(GLuint program);
};

// Helper function to create a full-screen quad for rendering
GLuint createFullScreenQuad();

#endif // SHADER_MANAGER_H