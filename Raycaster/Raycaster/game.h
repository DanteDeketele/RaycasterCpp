#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

class Game
{
public:

    Game(int width, int height, const char* title)
		: _width(width), _height(height), _title(title) {}
    void Run()
    {
        Initialize();

        while (!glfwWindowShouldClose(_window))
            Frame();
        Shutdown();
    }

private:
    void Initialize();
    bool LoadWindow(bool& retFlag);
    void HideCursor(bool value);
    void PrintBootMessage();
    void PrintRendererBootMessage();

    void Shutdown();

    void PrintShutdownMessage();

    void Frame();
    void compileShaders();
    GLuint loadImage(const std::string& filePath);
    void LoadMapToGpu(uint8_t mapData[16][16]);
    void processInput(GLFWwindow* window, double deltaTime, uint8_t mapData[16][16]);
    void setupBuffers();
    std::string loadShaderFromFile(const std::string& filePath);

    const char* _title = "Raycaster";

    int _width = 1920;
    int _height = 1080;

    GLFWwindow* _window = NULL;

    // FPS system
    double frameTimes[120] = {};
    float frameScreenshot[200] = {};
    int frameTimeIndex = 0;
    float fps = 0;
    double lastFrameTime = 0;

    const int MAP_WIDTH = 16;
    const int MAP_HEIGHT = 16;
    uint8_t mapData[16][16] = {
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


    // Define PI
    const float PI = 3.14159265359f;

    // Player state
    double playerPosX = 4.5f;
    double playerPosY = 4.5f;
    double playerAngle = 0.0f;
    double playerRadius = 0.2f;

    // Shaders
    GLuint shaderProgram;
    GLuint VAO, VBO;

    GLuint mapTexture;
    GLuint wallTexture;
    int wallTextureX = 6;
    int wallTextureY = 20;

    GLuint overlayTexture;
    GLuint skyTexture;
};