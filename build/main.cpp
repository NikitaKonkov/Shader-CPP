#include "..\SDL\include\SDL.h"
#include "..\GL\glew.h"
#include "..\SDL\include\SDL_opengl.h"
#include <iostream>
#include <string>

// Window dimensions
const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

// Vertex shader source
const GLchar* vertexShaderSource = R"(
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

// Fragment shader source (your ShaderToy shader)
const GLchar* fragmentShaderSource = R"(
    #version 330 core
    in vec2 fragCoord;
    out vec4 fragColor;
    
    uniform vec3 iResolution;
    uniform float iTime;
    uniform float iTimeDelta;
    uniform int iFrame;
    uniform vec4 iMouse;
    
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
    
    void main() {
        mainImage(fragColor, fragCoord * iResolution.xy);
    }
)";

// Function prototypes
bool initOpenGL();
GLuint createShaderProgram(const GLchar* vertexSource, const GLchar* fragmentSource);
void render(GLuint shaderProgram, GLuint VAO, float time, float deltaTime, int frame, float mouseX, float mouseY);
void cleanup(GLuint shaderProgram, GLuint VAO, GLuint VBO, GLuint EBO);

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
    
    // Create shader program
    GLuint shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);
    if (shaderProgram == 0) {
        SDL_GL_DeleteContext(glContext);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    // Set up vertex data for a full-screen quad
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
    
    // Bind VBO and copy vertex data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // Bind EBO and copy index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // Set vertex attribute pointers
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // TexCoord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Unbind VAO
    glBindVertexArray(0);
    
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
        
        // Render
        render(shaderProgram, VAO, time, deltaTime, frame, mouseX, mouseY);
        
        // Swap buffers
        SDL_GL_SwapWindow(window);
        
        // Increment frame counter
        frame++;
    }
    
    // Clean up
    cleanup(shaderProgram, VAO, VBO, EBO);
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}

GLuint createShaderProgram(const GLchar* vertexSource, const GLchar* fragmentSource) {
    // Create vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
    
    // Check for vertex shader compile errors
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        glDeleteShader(vertexShader);
        return 0;
    }
    
    // Create fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);
    
    // Check for fragment shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return 0;
    }
    
    // Create shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    // Check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(shaderProgram);
        return 0;
    }
    
    // Delete shaders as they're linked into the program and no longer needed
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return shaderProgram;
}

void render(GLuint shaderProgram, GLuint VAO, float time, float deltaTime, int frame, float mouseX, float mouseY) {
    // Clear the screen
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Use shader program
    glUseProgram(shaderProgram);
    
    // Set uniform values
    glUniform3f(glGetUniformLocation(shaderProgram, "iResolution"), (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT, 1.0f);
    glUniform1f(glGetUniformLocation(shaderProgram, "iTime"), time);
    glUniform1f(glGetUniformLocation(shaderProgram, "iTimeDelta"), deltaTime);
    glUniform1i(glGetUniformLocation(shaderProgram, "iFrame"), frame);
    glUniform4f(glGetUniformLocation(shaderProgram, "iMouse"), mouseX, mouseY, 0.0f, 0.0f);
    
    // Draw the quad
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void cleanup(GLuint shaderProgram, GLuint VAO, GLuint VBO, GLuint EBO) {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
}