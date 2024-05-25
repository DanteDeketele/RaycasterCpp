#include "Window.h"

int __stdcall WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow
)
{
	Window window(1280, 600, "Raycaster");

	while (true)
	{
		if (const auto msgCode = Window::ProcessMessages())
		{
			return *msgCode;
		}
	}
}