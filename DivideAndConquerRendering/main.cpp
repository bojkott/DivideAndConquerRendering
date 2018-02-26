//#include <SDL_keyboard.h>
//#include <SDL_events.h>
//#include <SDL_timer.h>
//#include <type_traits> 
//#include <assert.h>
#include <string>
#include "Common.h"
#include "Window.h"
#include "Renderer.h"

Window* window;
Renderer* renderer;

char titleBuff[256];
double lastDelta = 0.0;


void updateDelta()
{
#define WINDOW_SIZE 10
	static Uint64 start = 0;
	static Uint64 last = 0;
	static double avg[WINDOW_SIZE] = { 0.0 };
	static double lastSum = 10.0;
	static int loop = 0;

	last = start;
	start = SDL_GetPerformanceCounter();
	double deltaTime = (double)((start - last) * 1000.0 / SDL_GetPerformanceFrequency());
	// moving average window of WINDOWS_SIZE
	lastSum -= avg[loop];
	lastSum += deltaTime;
	avg[loop] = deltaTime;
	loop = (loop + 1) % WINDOW_SIZE;
	lastDelta = (lastSum / WINDOW_SIZE);
};

void run() {

	SDL_Event windowEvent;
	while (true)
	{
		if (SDL_PollEvent(&windowEvent))
		{
			if (windowEvent.type == SDL_QUIT) break;
			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_ESCAPE) break;
		}
		//updateScene();
		//renderScene();
		renderer->render();
		updateDelta();
		sprintf_s(titleBuff, "Vulkan - %3.0lf", lastDelta);
		SDL_SetWindowTitle(window->window, titleBuff);
	}
}

#undef main SDL_main	//Beacuse SDL_main was defined as main...
int main()
{
	try {
		window = new Window(800, 600);
		renderer = new Renderer();
		run();
	}
	catch (std::exception& e)
	{
		std::cout << "Error: " << e.what() << std::endl;
		system("pause");
	}

	return 0;
}
