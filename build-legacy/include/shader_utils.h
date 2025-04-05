#ifndef SHADER_UTILS_H
#define SHADER_UTILS_H

#include "gl_includes.h"

// Add these includes at the top of shader_utils.h
#ifdef _WIN32
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

// Update your loadShaderFromFile function
std::string loadShaderFromFile(const std::string& filePath) {
    // Print current working directory and file path for debugging
    char currentDir[FILENAME_MAX];
    if (GetCurrentDir(currentDir, FILENAME_MAX)) {
        std::cout << "Current working directory: " << currentDir << std::endl;
    }
    std::cout << "Attempting to load shader from: " << filePath << std::endl;
    
    // Check if file exists
    std::ifstream fileCheck(filePath);
    if (!fileCheck.good()) {
        std::cerr << "ERROR: File does not exist or cannot be accessed: " << filePath << std::endl;
        return "";
    }
    fileCheck.close();
    
    std::string shaderCode;
    std::ifstream shaderFile;
    
    // Ensure ifstream objects can throw exceptions
    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    
    try {
        // Open file
        shaderFile.open(filePath);
        std::stringstream shaderStream;
        
        // Read file's buffer contents into stream
        shaderStream << shaderFile.rdbuf();
        
        // Close file
        shaderFile.close();
        
        // Convert stream into string
        shaderCode = shaderStream.str();
        std::cout << "Successfully loaded shader from: " << filePath << std::endl;
    }
    catch (std::ifstream::failure& e) {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << filePath << std::endl;
        std::cerr << "Exception: " << e.what() << std::endl;
        return "";
    }
    
    return shaderCode;
}

// Function to check shader compilation/linking errors
void checkShaderError(GLuint shader, const std::string& type) {
    GLint success;
    GLchar infoLog[1024];
    
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" 
                      << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" 
                      << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}

// Function to create and compile a shader from file
GLuint createShaderFromFile(const std::string& filePath, GLenum shaderType) {
    // Load shader code from file
    std::string shaderCode = loadShaderFromFile(filePath);
    if (shaderCode.empty()) {
        return 0;
    }
    
    // Create and compile shader
    const char* shaderCodeCStr = shaderCode.c_str();
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderCodeCStr, NULL);
    glCompileShader(shader);
    
    // Check for compilation errors
    std::string typeName;
    switch (shaderType) {
        case GL_VERTEX_SHADER: typeName = "VERTEX"; break;
        case GL_FRAGMENT_SHADER: typeName = "FRAGMENT"; break;
        default: typeName = "UNKNOWN"; break;
    }
    checkShaderError(shader, typeName);
    
    return shader;
}

// Function to create a shader program from vertex and fragment shader files
GLuint createShaderProgram(const std::string& vertexPath, const std::string& fragmentPath) {
    // Create and compile shaders
    GLuint vertexShader = createShaderFromFile(vertexPath, GL_VERTEX_SHADER);
    GLuint fragmentShader = createShaderFromFile(fragmentPath, GL_FRAGMENT_SHADER);
    
    if (vertexShader == 0 || fragmentShader == 0) {
        return 0;
    }
    
    // Create and link shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    // Check for linking errors
    checkShaderError(shaderProgram, "PROGRAM");
    
    // Delete shaders as they're linked into the program and no longer needed
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return shaderProgram;
}

#endif // SHADER_UTILS_H