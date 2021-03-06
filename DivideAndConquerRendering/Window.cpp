#include "Window.h"
#include <stdio.h>
#include <stdlib.h>


SDL_Window* Window::window;
uint32_t Window::height;
uint32_t Window::width;

Window::Window(int width, int height)
{
	this->width = width;
	this->height = height;
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		fprintf(stderr, "%s", SDL_GetError());
		exit(-1);
	}
	window = SDL_CreateWindow("Divide&Conquer: Zero Overhead", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_VULKAN);
}

Window::~Window()
{
}

