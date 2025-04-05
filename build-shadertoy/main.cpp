#include "shader_manager.h"
#include "shadertoy_utils.h"
#include "shader_loader.h"

// Window dimensions
const int WINDOW_WIDTH = 1440;
const int WINDOW_HEIGHT = 720;

// Number of shaders
const int NUM_SHADERS = 11;

// Shader names for console output - IMPORTANT: Make sure this matches NUM_SHADERS!
const std::vector<std::string> SHADER_NAMES = {
    "cubes",
    "particles",
    "oldschool tube",
    "shader 4",
    "shader 5",
    "shader 6",
    "shader 7",
    "shader 8",
    "shader 9",
    "shader 10",
    "shader 11"
};

// Function to get key name for display
std::string getKeyName(int shaderIndex) {
    if (shaderIndex < 9) {
        // Use number keys 1-9 for first 9 shaders
        return std::to_string(shaderIndex + 1);
    } else {
        // Use letter keys A-Z for shaders 10 and beyond
        // A = shader 10, B = shader 11, etc.
        char letter = 'A' + (shaderIndex - 9);
        return std::string(1, letter);
    }
}

int main(int argc, char* argv[]) {
    // Validate shader configuration
    if (SHADER_NAMES.size() != NUM_SHADERS) {
        std::cerr << "ERROR: Number of shader names (" << SHADER_NAMES.size()
                  << ") doesn't match NUM_SHADERS (" << NUM_SHADERS << ")" << std::endl;
        return 1;
    }

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    
    // Set OpenGL attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    
    // Create window
    SDL_Window* window = SDL_CreateWindow(
        "ShaderToy Renderer",
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
    
    // Print key mapping information
    std::cout << "Shader Key Mappings:" << std::endl;
    for (int i = 0; i < NUM_SHADERS; i++) {
        std::cout << "  " << getKeyName(i) << " -> " << SHADER_NAMES[i] << std::endl;
    }
    
    // Load shader code from files
    std::vector<std::string> shaderCodes;
    for (int i = 1; i <= NUM_SHADERS; i++) {
        std::string shaderPath = "shaders/shader" + std::to_string(i) + ".glsl";
        std::cout << "Loading shader from: " << shaderPath << std::endl;
        
        std::string code = loadShaderFromFile(shaderPath);
        if (code.empty()) {
            std::cerr << "Failed to load shader" << i << ".glsl!" << std::endl;
            SDL_GL_DeleteContext(glContext);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return 1;
        }
        shaderCodes.push_back(code);
    }
    
    // Create shader managers and compile shaders
    std::vector<ShaderManager> shaderManagers(NUM_SHADERS);
    for (int i = 0; i < NUM_SHADERS; i++) {
        std::cout << "Compiling shader " << (i+1) << "..." << std::endl;
        std::string fragmentShaderSource = createShaderToyFragmentShader(shaderCodes[i]);
        if (!shaderManagers[i].loadFromStrings(defaultVertexShader, fragmentShaderSource)) {
            std::cerr << "Failed to load shader " << (i+1) << "!" << std::endl;
            SDL_GL_DeleteContext(glContext);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return 1;
        }
    }
    
    std::cout << "All " << NUM_SHADERS << " shaders compiled successfully!" << std::endl;
    
    // Create full-screen quad
    GLuint quadVAO = createFullScreenQuad();
    
    // Main loop flag
    bool quit = false;
    SDL_Event e;
    
    // For timing
    Uint32 currentTime = SDL_GetTicks();
    Uint32 lastTime = currentTime;
    float deltaTime = 0.0f;
    int frame = 0;
    
    // Mouse position
    int mouseX = 0, mouseY = 0;
    bool mouseDown = false;
    
    // Active shader (0-based index)
    int activeShader = 0;
    std::cout << "Starting with shader 1 (" << SHADER_NAMES[activeShader] << ")" << std::endl;
    
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
                // Handle shader switching with number keys (1-9)
                else if (e.key.keysym.sym >= SDLK_1 && e.key.keysym.sym <= SDLK_9) {
                    int newShader = e.key.keysym.sym - SDLK_1;
                    if (newShader < NUM_SHADERS) {
                        activeShader = newShader;
                        std::cout << "Switched to shader " << getKeyName(activeShader)
                                  << " (" << SHADER_NAMES[activeShader] << ")" << std::endl;
                    }
                }
                // Handle shader switching with letter keys (A-Z for shaders 10-35)
                else if (e.key.keysym.sym >= SDLK_a && e.key.keysym.sym <= SDLK_z) {
                    int newShader = (e.key.keysym.sym - SDLK_a) + 9; // A = shader 10, B = shader 11, etc.
                    if (newShader < NUM_SHADERS) {
                        activeShader = newShader;
                        std::cout << "Switched to shader " << getKeyName(activeShader)
                                  << " (" << SHADER_NAMES[activeShader] << ")" << std::endl;
                    }
                }
            }
            else if (e.type == SDL_MOUSEMOTION) {
                SDL_GetMouseState(&mouseX, &mouseY);
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    mouseDown = true;
                    SDL_GetMouseState(&mouseX, &mouseY);
                }
            }
            else if (e.type == SDL_MOUSEBUTTONUP) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    mouseDown = false;
                }
            }
        }
        
        // Calculate time
        lastTime = currentTime;
        currentTime = SDL_GetTicks();
        deltaTime = (currentTime - lastTime) / 1000.0f;
        float time = currentTime / 1000.0f;
        
        // Clear the screen
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Use the active shader and set uniforms
        if (activeShader >= 0 && activeShader < NUM_SHADERS) {
            shaderManagers[activeShader].use();
            shaderManagers[activeShader].setupShaderToyUniforms(
                WINDOW_WIDTH, WINDOW_HEIGHT, time, deltaTime, frame, mouseX, mouseY, mouseDown
            );
        }
        
        // Draw the quad
        glBindVertexArray(quadVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        
        // Swap buffers
        SDL_GL_SwapWindow(window);
        
        // Increment frame counter
        frame++;
        
        // Add a small delay to reduce CPU usage
        SDL_Delay(16); // ~60 FPS
    }
    
    // Clean up
    glDeleteVertexArrays(1, &quadVAO);
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}