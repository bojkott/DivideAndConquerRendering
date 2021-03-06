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
bool perModelRendering = true;
std::vector<Model*> models;

void updateDelta()
{
static Uint64 start = 0;
static Uint64 last = 0;

	last = start;
	start = SDL_GetPerformanceCounter();
	lastDelta = (double)((start - last) * 1000.0 / SDL_GetPerformanceFrequency());
};


void pmRendering()
{
	DeviceGroup* deviceGroup = &renderer->deviceGroup;
	int deviceIndex = 0;
	std::map<DeviceContext*, int> numberOfModelsOnDevices;
	for (auto& device : deviceGroup->getDevices())
	{
		numberOfModelsOnDevices[device] = models.size() * device->getLoadPercentage();
	}
	int modelIndex = 0;
	for (Model* m : models)
	{
		modelIndex++;
		DeviceContext* device = deviceGroup->getDevices()[deviceIndex];
		m->submitModel(device);
		if (modelIndex > numberOfModelsOnDevices[device])
		{
			if (renderer->getSlaveDevicesEnabled())
				deviceIndex = deviceGroup->getGroupSize() - 1;
			modelIndex = 0;
		}

	}
}

void run() {

	SDL_Event windowEvent;
	
	while (true)
	{
		if (SDL_PollEvent(&windowEvent))
		{
			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_SPACE)
			{
					renderer->toggleSlaveDevices(!renderer->getSlaveDevicesEnabled());
			}
			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_p)
			{
				perModelRendering = !perModelRendering;
			}
			if (windowEvent.type == SDL_QUIT) break;
			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_ESCAPE) break;
		}
		camera->update(lastDelta/1000.0f);

		if (perModelRendering)
			pmRendering();
		else
		{
			for (Model* m : models)
				m->submitModel(renderer);
		}
		

		renderer->render();
		updateDelta();
		sprintf_s(titleBuff, "Divide & Conquer Rendering [Vulkan] - frametime: %3.2lf, transfertime: %3.2lf, perModelRendering: %i", lastDelta, renderer->getTransferTime(), perModelRendering);

		printf("Frametime: %2.8f\n", lastDelta);

		SDL_SetWindowTitle(window->window, titleBuff);
	}
}

#undef main SDL_main	//Beacuse SDL_main was defined as main...
int main()
{


	try {
		window = new Window(1280, 720);
		renderer = Renderer::getInstance();
		camera = Camera::getInstance();

		Model* model = new Model("sponza.obj");
		models.push_back(model);
		for (int i = 1; i < 10; i++)
		{
			Model* m = new Model(model);
			m->setPosition(glm::vec3((i/5)*3500, 0, (i%5) * 2000));
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
