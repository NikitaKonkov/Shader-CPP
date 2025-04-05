#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#include "../include/includes.h"
#include "../include/utils.h"

// Window dimensions - changed to variables instead of constants
int WINDOW_WIDTH = 1024;
int WINDOW_HEIGHT = 1024;

// Default texture path
std::string texturePath = "textures/stone.png";

// Current shader paths
std::string currentVertexPath = "shaders/shader1/vertex.glsl";
std::string currentFragmentPath = "shaders/shader1/fragment.glsl";

// Function to handle window resize
void handleResize(int width, int height) {
    WINDOW_WIDTH = width;
    WINDOW_HEIGHT = height;
    
    // Update the OpenGL viewport to match the new window size
    glViewport(0, 0, width, height);
    
    std::cout << "Window resized to: " << width << "x" << height << std::endl;
}

// Function to load and compile shaders
GLuint loadShaders(const std::string& vertexPath, const std::string& fragmentPath) {
    GLuint shaderProgram = createShaderProgram(vertexPath, fragmentPath);
    if (shaderProgram == 0) {
        std::cerr << "Failed to create shader program!" << std::endl;
    }
    return shaderProgram;
}

// Function to reload the current shader
GLuint reloadCurrentShader(GLuint currentProgram) {
    // Delete the current program
    if (currentProgram != 0) {
        glDeleteProgram(currentProgram);
    }
    
    // Load and compile the updated shader
    std::cout << "Reloading shaders from: " << currentVertexPath << " and " << currentFragmentPath << std::endl;
    GLuint newProgram = loadShaders(currentVertexPath, currentFragmentPath);
    
    if (newProgram == 0) {
        std::cerr << "Shader reload failed! Keeping previous shader." << std::endl;
        // If reload fails, try to recreate the original shader
        newProgram = loadShaders(currentVertexPath, currentFragmentPath);
    } else {
        std::cout << "Shader reloaded successfully!" << std::endl;
    }
    
    return newProgram;
}

// Function to load a texture from file
GLuint loadTexture(const std::string& path) {
    // Initialize SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
        return 0;
    }
    
    // Load image
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) {
        std::cerr << "Unable to load image " << path << "! SDL_image Error: " << IMG_GetError() << std::endl;
        return 0;
    }
    
    // Create texture from surface pixels
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // Determine format
    GLenum format;
    if (surface->format->BytesPerPixel == 4) {
        format = (surface->format->Rmask == 0x000000ff) ? GL_RGBA : GL_BGRA;
    } else if (surface->format->BytesPerPixel == 3) {
        format = (surface->format->Rmask == 0x000000ff) ? GL_RGB : GL_BGR;
    } else {
        std::cerr << "Warning: Image is not truecolor!" << std::endl;
        format = GL_RGB;
    }
    
    // Upload pixels to texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, format, GL_UNSIGNED_BYTE, surface->pixels);
    
    // Free surface
    SDL_FreeSurface(surface);
    
    return textureID;
}

int drawer(int argc, char* argv[]) {
    // Check if texture path is provided as command-line argument
    if (argc > 1) {
        texturePath = argv[1];
        std::cout << "Using texture: " << texturePath << std::endl;
    } else {
        std::cout << "No texture specified, using default: " << texturePath << std::endl;
    }

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    
    // Set OpenGL attributes - IMPORTANT: Request compatibility profile
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    
    // Create window with resizable flag
    SDL_Window* window = SDL_CreateWindow(
        "Legacy GLSL Shader Demo",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }
    
    // Create OpenGL context
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        std::cerr << "OpenGL context could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    // Initialize GLEW
    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK) {
        std::cerr << "GLEW could not be initialized! Error: " << glewGetErrorString(glewError) << std::endl;
        SDL_GL_DeleteContext(glContext);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    // Print OpenGL version
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    
    // Load background texture
    GLuint backgroundTexture = loadTexture(texturePath);
    if (backgroundTexture == 0) {
        std::cerr << "Failed to load background texture: " << texturePath << std::endl;
        // Continue without texture
    }
    
    // Load initial shader program
    GLuint shaderProgram = loadShaders(currentVertexPath, currentFragmentPath);
    if (shaderProgram == 0) {
        SDL_GL_DeleteContext(glContext);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    // Set up vertex data (a quad made of two triangles)
    float vertices[] = {
        // positions        // texture coords
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,  // top left
        1.0f,  1.0f, 0.0f, 1.0f, 1.0f,  // top right
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,  // bottom right
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f   // bottom left
    };
    
    unsigned int indices[] = {
        0, 1, 2,  // first triangle
        0, 2, 3   // second triangle
    };
    
    // Create VAO, VBO, and EBO
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    // Bind VAO
    glBindVertexArray(VAO);
    
    // Bind and set VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // Bind and set EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // Get attribute locations
    GLint posAttrib = glGetAttribLocation(shaderProgram, "aPosition");
    GLint texAttrib = glGetAttribLocation(shaderProgram, "aTexCoord");
    
    // Set vertex attribute pointers
    // Position attribute
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(posAttrib);
    
    // Texture coordinate attribute
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(texAttrib);
    
    // Unbind VAO
    glBindVertexArray(0);
    
    // Get the location of the uniforms
    GLint millisLoc = glGetUniformLocation(shaderProgram, "millis");
    GLint backgroundLoc = glGetUniformLocation(shaderProgram, "background");
    
    // Initialize viewport
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    
    // Main loop flag
    bool quit = false;
    SDL_Event e;
    
    // For timing
    Uint32 startTime = SDL_GetTicks();
    
    // Main loop
    while (!quit) {
        // Handle events
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) {
                    quit = true;
                }
                else if (e.key.keysym.sym == SDLK_1) {
                    // Switch to shader 1
                    currentVertexPath = "shaders/shader1/vertex.glsl";
                    currentFragmentPath = "shaders/shader1/fragment.glsl";
                    shaderProgram = reloadCurrentShader(shaderProgram);
                    // Get the uniform locations again after reloading
                    millisLoc = glGetUniformLocation(shaderProgram, "millis");
                    backgroundLoc = glGetUniformLocation(shaderProgram, "background");
                }
                else if (e.key.keysym.sym == SDLK_2) {
                    // Switch to shader 2
                    currentVertexPath = "shaders/shader2/vertex.glsl";
                    currentFragmentPath = "shaders/shader2/fragment.glsl";
                    shaderProgram = reloadCurrentShader(shaderProgram);
                    // Get the uniform locations again after reloading
                    millisLoc = glGetUniformLocation(shaderProgram, "millis");
                    backgroundLoc = glGetUniformLocation(shaderProgram, "background");
                }
                else if (e.key.keysym.sym == SDLK_r) {
                    // Reload current shader
                    shaderProgram = reloadCurrentShader(shaderProgram);
                    // Update attribute and uniform locations after reload
                    posAttrib = glGetAttribLocation(shaderProgram, "aPosition");
                    texAttrib = glGetAttribLocation(shaderProgram, "aTexCoord");
                    millisLoc = glGetUniformLocation(shaderProgram, "millis");
                    backgroundLoc = glGetUniformLocation(shaderProgram, "background");
                    // Rebind VAO
                    glBindVertexArray(VAO);
                    // Update vertex attribute pointers
                    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
                    glEnableVertexAttribArray(posAttrib);
                    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
                    glEnableVertexAttribArray(texAttrib);
                    // Unbind VAO
                    glBindVertexArray(0);
                }
            }
            // Handle window resize event
            else if (e.type == SDL_WINDOWEVENT) {
                if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
                    // Handle window resize
                    handleResize(e.window.data1, e.window.data2);
                }
            }
        }
        
        // Calculate elapsed time in milliseconds
        float currentTime = SDL_GetTicks() - startTime;
        
        // Clear the screen
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Use the shader program
        glUseProgram(shaderProgram);
        
        // Activate texture unit 0 and bind the background texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, backgroundTexture);
        
        // Set the background uniform to use texture unit 0
        if (backgroundLoc != -1) {
            glUniform1i(backgroundLoc, 0);
        }
        
        // Update the millis uniform
        if (millisLoc != -1) {
            glUniform1f(millisLoc, currentTime);
        }
        
        // Draw the quad
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        
        // Swap buffers
        SDL_GL_SwapWindow(window);
        
        // Add a small delay to reduce CPU usage
        SDL_Delay(16); // ~60 FPS
    }
    
    // Clean up
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}


#endif // SHADER_MANAGER_H