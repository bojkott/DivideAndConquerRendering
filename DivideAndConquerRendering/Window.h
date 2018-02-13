#pragma once

#include <SDL.h>

#pragma comment(lib, "vulkan-1.lib")
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")

/*
*	Class for handling the window
*/
class Window
{
public:

private:
	static uint32_t height;
	static uint32_t width;
public:
	Window(int height, int width);
	~Window();
	static SDL_Window* window;

	static uint32_t getWidth() { return width; }
	static uint32_t getHeight() { return height; }

private:


};
