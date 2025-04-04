#include "shader_manager.h"
#include "shadertoy_utils.h"

// Window dimensions
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

// ShaderToy fragment shader code
const std::string shaderToyCode = R"(
float segment(vec2 p, vec2 a, vec2 b) {
    p -= a;
    b -= a;
    return length(p - b * clamp(dot(p, b) / dot(b, b), 0., 1.));
}

#define rot(a) mat2(cos(a+vec4(0,1.57,-1.57,0)))

float t;
vec2 T(vec3 p) {
    p.xy *= rot(-t);
    p.xz *= rot(.785);
    p.yz *= rot(-.625);
    return p.xy;
}

void mainImage(out vec4 O, vec2 u) {
    vec2 R = iResolution.xy, X,
    U = 10. * u / R.y,
    M = vec2(2,2.3),
    I = floor(U/M)*M, J;
    U = mod(U, M);
    O *= 0.;
    for (int k=0; k<4; k++) {
        X = vec2(k%2,k/2)*M;
        J = I+X;
        if (int(J/M)%2 > 0) X.y += 1.15;
        t = tanh(-.2*(J.x+J.y) + mod(2.*iTime,10.) -1.6)*.785;
        for (float a=0; a < 6.; a += 1.57) {
            vec3 A = vec3(cos(a),sin(a),.7),
            B = vec3(-A.y,A.x,.7);
            #define L(A,B) O += smoothstep(15./R.y, 0., segment(U-X, T(A), T(B)))
            L(A,B);
            L(A,A*vec3(1,1,-1));
            A.z=-A.z; B.z=-B.z; L(A,B);
        }
    }
}
)";

int main(int argc, char* argv[]) {
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
    
    // Create shader manager
    ShaderManager shaderManager;
    
    // Create ShaderToy fragment shader
    std::string fragmentShaderSource = createShaderToyFragmentShader(shaderToyCode);
    
    // Load shaders
    if (!shaderManager.loadFromStrings(defaultVertexShader, fragmentShaderSource)) {
        std::cerr << "Failed to load shaders!" << std::endl;
        SDL_GL_DeleteContext(glContext);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
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
        
        // Use shader and set uniforms
        shaderManager.use();
        shaderManager.setupShaderToyUniforms(WINDOW_WIDTH, WINDOW_HEIGHT, time, deltaTime, frame, mouseX, mouseY, mouseDown);
        
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