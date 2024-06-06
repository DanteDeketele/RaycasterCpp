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

    const char* _title = "Raycaster";

    int _width = 1920;
    int _height = 1080;

    GLFWwindow* _window = NULL;
};