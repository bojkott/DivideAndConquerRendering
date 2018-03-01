//#include <SDL_keyboard.h>
//#include <SDL_events.h>
//#include <SDL_timer.h>
//#include <type_traits> 
//#include <assert.h>
#include <string>
#include <iostream>
#include "Renderer.h"

Renderer* renderer;

char titleBuff[256];
double lastDelta = 0.0;



void run() {

	while (true)
	{
		renderer->render();
		printf("Vulkan - frametime: %3.0lf, transfertime: %3.0lf\n", lastDelta, renderer->getTransferTime());
	}
}

#undef main SDL_main	//Beacuse SDL_main was defined as main...
int main()
{
	try {
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
