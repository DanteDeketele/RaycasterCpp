#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <fstream>
#include <sstream>

// Define window dimensions
const unsigned int WIDTH = 1920;
const unsigned int HEIGHT = 1080;

// Define map dimensions
const int MAP_WIDTH = 16;
const int MAP_HEIGHT = 16;

// Define PI
const float PI = 3.14159265359f;

// Player state
float playerPosX = 4.5f;
float playerPosY = 4.5f;
float playerAngle = 0.0f;

// FPS system
float frameTimes[60];
int frameTimeIndex = 0;

// Shaders
GLuint shaderProgram;
GLuint VAO, VBO;

GLuint mapTexture;

// Function prototypes
void processInput(GLFWwindow* window, double deltaTime);
void compileShaders();
void setupBuffers();
void LoadMapToGpu(uint8_t mapData[MAP_WIDTH][MAP_HEIGHT]);

int main()
{
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    // Create a GLFWwindow object
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Raycaster", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Log game info
    std::cout << "--------------------------------" << std::endl;
    std::cout << "Raycaster" << std::endl;
    std::cout << "Press WASD to move" << std::endl;
    std::cout << "Move mouse to look around" << std::endl;
    std::cout << "Press ESC to exit" << std::endl;
    std::cout << "--------------------------------" << std::endl;
    // Log the window info to console
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLFW version: " << glfwGetVersionString() << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "Window size: " << WIDTH << "x" << HEIGHT << std::endl;
    std::cout << "Window handle: " << window << std::endl;

    // Hide the mouse cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW\n";
        return -1;
    }

    compileShaders();
    setupBuffers();

    // Time variables
    double lastFrameTime = glfwGetTime();

    // Log start gameloop
    std::cout << "--------------------------------" << std::endl;
    std::cout << "Starting game loop" << std::endl;

    uint8_t mapData[MAP_WIDTH][MAP_HEIGHT] = {
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
    };

    // print map
    std::cout << "Map:" << std::endl;
    for (int y = 0; y < MAP_HEIGHT; y++) {
		for (int x = 0; x < MAP_WIDTH; x++) {
			std::cout << (int)mapData[x][y] << " ";
		}
		std::cout << std::endl;
	}

    // Load map data to GPU
    LoadMapToGpu(mapData);

    // print start data
    std::cout << "Player position: (" << playerPosX << ", " << playerPosY << ")" << std::endl;
    std::cout << "Player angle: " << playerAngle << std::endl;


    // Main loop
    while (!glfwWindowShouldClose(window) && !(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS))
    {
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastFrameTime;
        lastFrameTime = currentTime;

        // FPS counter
        frameTimes[frameTimeIndex] = (float)deltaTime;
        frameTimeIndex = (frameTimeIndex + 1) % 60;
        if (frameTimeIndex == 0) {
			double sum = 0;
			for (int i = 0; i < 60; i++) {
				sum += frameTimes[i];
			}
			double fps = 60.0 / sum;
			std::cout << "FPS: " << fps << std::endl;
		}

        processInput(window, deltaTime);

        // Render here
        glClear(GL_COLOR_BUFFER_BIT);

        // Use the shader program
        glUseProgram(shaderProgram);

        // Bind the preloaded texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mapTexture);

        // Update uniforms that change every frame
        GLint resolutionLoc = glGetUniformLocation(shaderProgram, "uResolution");
        glUniform2f(resolutionLoc, WIDTH, HEIGHT);

        GLint playerPosLoc = glGetUniformLocation(shaderProgram, "uPlayerPos");
        glUniform2f(playerPosLoc, playerPosX, playerPosY);

        GLint playerAngleLoc = glGetUniformLocation(shaderProgram, "uPlayerAngle");
        glUniform1f(playerAngleLoc, playerAngle);

        GLint mapLoc = glGetUniformLocation(shaderProgram, "map");
        glUniform1i(mapLoc, 0); // Texture unit 0

        // Draw the quad
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //  Log game closing
    std::cout << "--------------------------------" << std::endl;
    std::cout << "Closing game" << std::endl;

    // Clean up
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void LoadMapToGpu(uint8_t mapData[MAP_WIDTH][MAP_HEIGHT]) {
    // Generate and bind a texture object
    glGenTextures(1, &mapTexture);
    glBindTexture(GL_TEXTURE_2D, mapTexture);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Transfer map data to the texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, MAP_WIDTH, MAP_HEIGHT, 0, GL_RED, GL_UNSIGNED_BYTE, mapData);
}



void processInput(GLFWwindow* window, double deltaTime)
{
    const float moveSpeed = 2.5f * (float)deltaTime; // Adjust movement speed with delta time
    const float turnSpeed = 0.075f * (float)deltaTime; // Adjust turn speed with delta time

    // Check if the window is focused before processing input
    if (glfwGetWindowAttrib(window, GLFW_FOCUSED))
    {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            playerPosX += cos(playerAngle) * moveSpeed;
            playerPosY += sin(playerAngle) * moveSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            playerPosX -= cos(playerAngle) * moveSpeed;
            playerPosY -= sin(playerAngle) * moveSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            playerPosX += cos(playerAngle - PI/2) * moveSpeed;
            playerPosY += sin(playerAngle - PI/2) * moveSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            playerPosX -= cos(playerAngle - PI / 2) * moveSpeed;
            playerPosY -= sin(playerAngle - PI / 2) * moveSpeed;
        }

        // Handle mouse movement
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        double deltaX = xpos - WIDTH / 2.0;
        double deltaY = ypos - HEIGHT / 2.0;

        // Adjust playerAngle based on mouse movement
        playerAngle += (float)deltaX * turnSpeed;

        // Clamp playerAngle to keep it within [0, 2*PI)
        while (playerAngle < 0)
            playerAngle += 2 * PI;
        while (playerAngle >= 2 * PI)
            playerAngle -= 2 * PI;
        
        // Center mouse in the window
        glfwSetCursorPos(window, WIDTH / 2.0, HEIGHT / 2.0);
    }
}

std::string loadShaderFromFile(const std::string& filePath) {
    std::ifstream shaderFile(filePath);
    std::stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();
    return shaderStream.str();
}

void compileShaders()
{
    // Load fragment shader code from file
    std::string fragmentShaderSource = loadShaderFromFile("fragment_shader.glsl").c_str();


    // Compile vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexShaderSource = R"glsl(
        #version 330 core
        layout(location = 0) in vec2 aPos;
        out vec2 TexCoord;
        void main()
        {
            gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
            TexCoord = aPos * 0.5 + 0.5;
        }
    )glsl";
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentShaderSourcePtr = fragmentShaderSource.c_str();
    glShaderSource(fragmentShader, 1, &fragmentShaderSourcePtr, NULL);
    glCompileShader(fragmentShader);

    // Check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    

    // Link shaders
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    

    // Check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void setupBuffers()
{
    // Vertex data for a fullscreen quad
    float vertices[] = {
        // positions
        -1.0f,  1.0f,
        -1.0f, -1.0f,
         1.0f, -1.0f,

        -1.0f,  1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}
