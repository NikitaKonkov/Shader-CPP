#ifndef SHADER_LOADER_H
#define SHADER_LOADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

// Function to load shader code from a file
std::string loadShaderFromFile(const std::string& filePath) {
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
    }
    catch (std::ifstream::failure& e) {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << filePath << std::endl;
        std::cerr << "Exception: " << e.what() << std::endl;
        return "";
    }
    
    return shaderCode;
}

#endif // SHADER_LOADER_H