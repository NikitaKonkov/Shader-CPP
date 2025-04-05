#include "../include/includes.h"
#include "../include/utils.h"

// Window dimensions
const int WINDOW_WIDTH = 600;
const int WINDOW_HEIGHT = 600;

// Current shader paths
std::string currentVertexPath = "shaders/shader1/vertex.glsl";
std::string currentFragmentPath = "shaders/shader1/fragment.glsl";

// Function to load and compile shaders
GLuint loadShaders(const std::string& vertexPath, const std::string& fragmentPath) {
    GLuint shaderProgram = createShaderProgram(vertexPath, fragmentPath);
    if (shaderProgram == 0) {
        std::cerr << "Failed to create shader program!" << std::endl;
    } else {
        std::cout << "Shader program created successfully!" << std::endl;
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

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    
    // Set OpenGL attributes - IMPORTANT: Request compatibility profile
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    
    // Create window
    SDL_Window* window = SDL_CreateWindow(
        "Legacy GLSL Shader Demo",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
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
    
    // Main loop flag
    bool quit = false;
    SDL_Event e;
    
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
                }
                else if (e.key.keysym.sym == SDLK_2) {
                    // Switch to shader 2
                    currentVertexPath = "shaders/shader2/vertex.glsl";
                    currentFragmentPath = "shaders/shader2/fragment.glsl";
                    shaderProgram = reloadCurrentShader(shaderProgram);
                }
                else if (e.key.keysym.sym == SDLK_r) {
                    // Reload current shader
                    shaderProgram = reloadCurrentShader(shaderProgram);
                    
                    // Update attribute locations after reload
                    posAttrib = glGetAttribLocation(shaderProgram, "aPosition");
                    texAttrib = glGetAttribLocation(shaderProgram, "aTexCoord");
                    
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
        }
        
        // Clear the screen
        glClearColor(1.f, 1.f, 1.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Use the shader program
        glUseProgram(shaderProgram);
        
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