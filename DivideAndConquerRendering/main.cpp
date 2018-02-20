//#include <SDL_keyboard.h>
//#include <SDL_events.h>
//#include <SDL_timer.h>
//#include <type_traits> 
//#include <assert.h>

#include "Window.h"
#include "Renderer.h"

Window* window;
Renderer* renderer;


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
	}
}

#undef main SDL_main	//Beacuse SDL_main was defined as main...
int main()
{
	window = new Window(800, 600);
	renderer = new Renderer();
	run();
	return 0;
}
