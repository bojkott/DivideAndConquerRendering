#pragma once

#include <SDL.h>

#pragma comment(lib, "vulkan-1.lib")
#pragma comment(lib,"SDL2.lib")
#pragma comment(lib,"SDL2main.lib")

/*
*	Class for handling the window
*/
class Window
{
public:

private:
	SDL_Window* window;

	uint32_t height;
	uint32_t width;
public:
	Window(int height, int width);
	~Window();

	SDL_Window* getWindow();
private:


};
