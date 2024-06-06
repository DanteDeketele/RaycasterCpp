#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include "game.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


#pragma region Initialization

void Game::Initialize()
{
	// Initialize subsystems

	PrintBootMessage();

    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Load window
    bool retFlag;
    bool retVal = LoadWindow(retFlag);
    if (retFlag)
        exit(EXIT_FAILURE);

    // Print renderer boot message
    PrintRendererBootMessage();

    // Hide cursor
    HideCursor(true);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW\n";
        exit(EXIT_FAILURE);
    }

    // print glew version
    std::cout << "GLEW version: " << glewGetString(GLEW_VERSION) << std::endl;

    compileShaders();
    setupBuffers();

    // Load the wall texture
    wallTexture = loadImage("images/sheet.png");
    overlayTexture = loadImage("images/overlay.png");
    skyTexture = loadImage("images/sky.png");

    // Load map data to GPU
    LoadMapToGpu(mapData);

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(_window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Time variables
    lastFrameTime = glfwGetTime();
}

bool Game::LoadWindow(bool& retFlag)
{
    retFlag = true;
    // Create a GLFWwindow object
    _window = glfwCreateWindow(_width, _height, "Raycaster", NULL, NULL);
    if (_window == NULL)
    {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(_window);

    // Ensure V-Sync is disabled
    glfwSwapInterval(0);
    retFlag = false;
    return {};
}

void Game::HideCursor(bool value)
{
    if (value)
		// Show the mouse cursor
		glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	else
        // Hide the mouse cursor
        glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void Game::PrintBootMessage()
{
	std::string startupMessage = "";
	startupMessage += "------ Raycaster ------\n";

	startupMessage += "Starting game with title: ";
	startupMessage += _title;
	startupMessage += "\n";

	startupMessage += "Resolution: ";
	startupMessage += std::to_string(_width);
	startupMessage += "x";
	startupMessage += std::to_string(_height);
	startupMessage += "\n";

	startupMessage += "Made by Dante Deketele\n";

	startupMessage += "-----------------------\n";

	std::cout << startupMessage;
}

void Game::PrintRendererBootMessage()
{
    std::string rendererLoadMessage = "Loading renderer: ";
    rendererLoadMessage += "OpenGL version: ";
    rendererLoadMessage += (char*)glGetString(GL_VERSION);
    rendererLoadMessage += "\n";
    rendererLoadMessage += "GLFW version: ";
    rendererLoadMessage += glfwGetVersionString();
    rendererLoadMessage += "\n";
    rendererLoadMessage += "Renderer: ";
    rendererLoadMessage += (char*)glGetString(GL_RENDERER);
    rendererLoadMessage += "\n";
    rendererLoadMessage += "Vendor: ";
    rendererLoadMessage += (char*)glGetString(GL_VENDOR);
    rendererLoadMessage += "\n";
    rendererLoadMessage += "GLSL version: ";
    rendererLoadMessage += (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
    rendererLoadMessage += "\n";
    rendererLoadMessage += "Window size: ";
    rendererLoadMessage += std::to_string(_width);
    rendererLoadMessage += "x";
    rendererLoadMessage += std::to_string(_height);
    rendererLoadMessage += "\n";
    rendererLoadMessage += "Window handle: ";
    rendererLoadMessage += std::to_string((int)_window);
    rendererLoadMessage += "\n";
    std::cout << rendererLoadMessage;
}

#pragma endregion

void Game::Frame()
{
    double currentTime = glfwGetTime();
    double deltaTime = currentTime - lastFrameTime;
    lastFrameTime = currentTime;

    // FPS counter
    frameTimes[frameTimeIndex] = deltaTime;
    frameTimeIndex = (frameTimeIndex + 1) % 120;
    if (frameTimeIndex == 0) {
        double sum = 0;
        for (int i = 0; i < 120; i++) {
            sum += frameTimes[i];
        }
        fps = (float)(120.0 / sum);

        
        for (int i = 200 - 1; i > 0; i--) {
			frameScreenshot[i] = frameScreenshot[i - 1];
		}
        frameScreenshot[0] = fps;
    }


    // Update and draw game
    glClear(GL_COLOR_BUFFER_BIT);

    processInput(_window, deltaTime, mapData);



    // Render here
    glClear(GL_COLOR_BUFFER_BIT);

    // Use the shader program
    glUseProgram(shaderProgram);

    // Bind the preloaded texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mapTexture);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, wallTexture);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, overlayTexture);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, skyTexture);

    // Update uniforms that change every frame
    GLint resolutionLoc = glGetUniformLocation(shaderProgram, "uResolution");
    glUniform2f(resolutionLoc, _width, _height);

    GLint playerPosLoc = glGetUniformLocation(shaderProgram, "uPlayerPos");
    glUniform2f(playerPosLoc, (GLfloat)playerPosX, (GLfloat)playerPosY);

    GLint playerAngleLoc = glGetUniformLocation(shaderProgram, "uPlayerAngle");
    glUniform1f(playerAngleLoc, (GLfloat)playerAngle);

    GLint mapLoc = glGetUniformLocation(shaderProgram, "map");
    glUniform1i(mapLoc, 0); // Texture unit 0

    GLint wallLoc = glGetUniformLocation(shaderProgram, "textures");
    glUniform1i(wallLoc, 1); // Texture unit 1

    // Bind wall TextureSheetSize
    GLint wallXLoc = glGetUniformLocation(shaderProgram, "texturesX");
    glUniform1i(wallXLoc, wallTextureX);

    GLint wallYLoc = glGetUniformLocation(shaderProgram, "texturesY");
    glUniform1i(wallYLoc, wallTextureY);

    GLint overlayLoc = glGetUniformLocation(shaderProgram, "overlay");
    glUniform1i(overlayLoc, 2); // Texture unit 2

    GLint skyLoc = glGetUniformLocation(shaderProgram, "skybox");
    glUniform1i(skyLoc, 3); // Texture unit 3

    // Draw the quad
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);



    // Start the ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    std::string fpsString = std::to_string(fps);

    ImGui::Begin("Very useful window for cool raycaster - Dante Deketele");
    ImGui::Text("FPS: %s", fpsString.c_str());
    ImGui::Text("Player position: (%f, %f)", playerPosX, playerPosY);
    ImGui::Text("Player angle: %f", playerAngle);

    float fpsArray[200];
    for (int i = 0; i < 200; i++) {
		fpsArray[i] = frameScreenshot[i];
	}

    ImGui::PlotLines("FPS", fpsArray, 200, 0, NULL, 0, 4000, ImVec2(0, 80));

    float msArray[200];
    float maxMs = 0;
    for (int i = 0; i < 200; i++) {
        msArray[i] = 1/frameScreenshot[i];
        if (msArray[i] > maxMs) {
			maxMs = msArray[i];
		}
    }

    ImGui::PlotLines("ms", msArray, 200, 0, NULL, 0, maxMs, ImVec2(0, 80));
    ImGui::End();


    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Swap buffers and poll events
    glfwSwapBuffers(_window);
    glfwPollEvents();
}

void Game::LoadMapToGpu(uint8_t mapData[16][16]) {
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



void Game::processInput(GLFWwindow* window, double deltaTime, uint8_t mapData[16][16])
{
    const double moveSpeed = 2.5f * deltaTime; // Adjust movement speed with delta time
    const double turnSpeed = 0.001f; // Adjust turn speed with delta time

    // Check if the window is focused before processing input
    if (glfwGetWindowAttrib(window, GLFW_FOCUSED))
    {
        double moveX = 0;
        double moveY = 0;

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            moveX += cos(playerAngle) * moveSpeed;
            moveY += sin(playerAngle) * moveSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            moveX -= cos(playerAngle) * moveSpeed;
            moveY -= sin(playerAngle) * moveSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            moveX += cos(playerAngle - PI / 2) * moveSpeed;
            moveY += sin(playerAngle - PI / 2) * moveSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            moveX -= cos(playerAngle - PI / 2) * moveSpeed;
            moveY -= sin(playerAngle - PI / 2) * moveSpeed;
        }

        if (playerPosX + moveX / abs(moveX) * playerRadius >= 0 && playerPosX + moveX / abs(moveX) * playerRadius < MAP_WIDTH && mapData[(int)(playerPosX + moveX / abs(moveX) * playerRadius)][(int)playerPosY] == 0)
            playerPosX += moveX;

        if (playerPosY + moveY / abs(moveY) * playerRadius >= 0 && playerPosY + moveY / abs(moveY) * playerRadius < MAP_HEIGHT && mapData[(int)playerPosX][(int)(playerPosY + moveY / abs(moveY) * playerRadius)] == 0)
            playerPosY += moveY;

        // Handle mouse movement
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        double deltaX = xpos - _width / 2.0;
        double deltaY = ypos - _height / 2.0;

        // Adjust playerAngle based on mouse movement
        playerAngle += deltaX * turnSpeed;

        // Clamp playerAngle to keep it within [0, 2*PI)
        while (playerAngle < 0)
            playerAngle += 2 * PI;
        while (playerAngle >= 2 * PI)
            playerAngle -= 2 * PI;

        // Center mouse in the window
        glfwSetCursorPos(window, _width / 2.0, _height / 2.0);
    }
}

std::string Game::loadShaderFromFile(const std::string& filePath) {
    std::ifstream shaderFile(filePath);
    std::stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();
    return shaderStream.str();
}

// Function to load an image file into a texture
GLuint Game::loadImage(const std::string& filePath)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Set texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Load image using stb_image
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // Flip image vertically as OpenGL expects the 0.0 coordinate to be at the bottom left corner
    unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format = GL_RGB;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;


        // Generate the texture
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cerr << "Failed to load texture: " << filePath << std::endl;
    }
    stbi_image_free(data);

    return textureID;
}

void Game::compileShaders()
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

void Game::setupBuffers()
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


#pragma region Shutdown

void Game::Shutdown()
{
    PrintShutdownMessage();

    // Clean up
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    // Cleanup ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(_window);
    glfwTerminate();
}

void Game::PrintShutdownMessage()
{
    //  Log game closing
    std::cout << "-----------------------" << std::endl;
    std::cout << "Closing game" << std::endl;
}

#pragma endregion