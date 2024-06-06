#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <sstream>
#include "game.h"


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

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(_window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
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
    // Update and draw game
    glClear(GL_COLOR_BUFFER_BIT);

    // Start the ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Example ImGui window
    ImGui::Begin("Hello, world!");
    ImGui::Text("This is some useful text.");
    ImGui::End();

    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Swap buffers and poll events
    glfwSwapBuffers(_window);
    glfwPollEvents();
}


#pragma region Shutdown

void Game::Shutdown()
{
    PrintShutdownMessage();

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