//#include <SDL_keyboard.h>
//#include <SDL_events.h>
//#include <SDL_timer.h>
//#include <type_traits> 
//#include <assert.h>

#include "Window.h"

Window* window;

#undef main SDL_main	//Beacuse SDL_main was defined as main...
int main()
{
	window = new Window(800, 600);
	return 0;
}