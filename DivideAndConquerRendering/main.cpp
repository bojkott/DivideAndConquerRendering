//#include <SDL_keyboard.h>
//#include <SDL_events.h>
//#include <SDL_timer.h>
//#include <type_traits> 
//#include <assert.h>
#include <string>
#include <iostream>
#include "Window.h"
#include "Renderer.h"
#include "Camera.h"

Window* window;
Renderer* renderer;
Camera* camera;
char titleBuff[256];
double lastDelta = 0.0;

std::vector<Model*> models;

void updateDelta()
{
static Uint64 start = 0;
static Uint64 last = 0;

	last = start;
	start = SDL_GetPerformanceCounter();
	lastDelta = (double)((start - last) * 1000.0 / SDL_GetPerformanceFrequency());
};

void run() {

	SDL_Event windowEvent;
	SDL_SetRelativeMouseMode(SDL_TRUE);
	while (true)
	{
		if (SDL_PollEvent(&windowEvent))
		{
			if (windowEvent.type == SDL_QUIT) break;
			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_ESCAPE) break;
			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_1) renderer->toggleSlaveDevices(!renderer->getSlaveDevicesEnabled());
		}
		camera->update(lastDelta/1000.0f);

		for (Model* m : models)
			m->submitModel(renderer);

		renderer->render();
		updateDelta();
		sprintf_s(titleBuff, "Divide & Conquer Rendering [Vulkan] - frametime: %3.2lf, transfertime: %3.2lf", lastDelta, renderer->getTransferTime());
		SDL_SetWindowTitle(window->window, titleBuff);
	}
}

#undef main SDL_main	//Beacuse SDL_main was defined as main...
int main()
{


	try {
		window = new Window(1280, 720);
		renderer = new Renderer();
		camera = Camera::getInstance();

		for (int i = 0; i < 1; i++)
		{
			Model* m = new Model("sponza.obj");
			m->setPosition(glm::vec3((i/5)*1000, 0, i * 500));
			models.push_back(m);
		}
		
		run();
	}
	catch (std::exception& e)
	{
		std::cout << "Error: " << e.what() << std::endl;
		system("pause");
	}

	return 0;
}
