#include <switch.h>
//#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "xplayer_fonts.h"

//
void load_fonts()
{
	Roboto = TTF_OpenFont("romfs:/resources/Roboto-Regular.ttf", 25);
	Roboto_large = TTF_OpenFont("romfs:/resources/Roboto-Regular.ttf", 35);
	Roboto_50 = TTF_OpenFont("romfs:/resources/Roboto-Regular.ttf", 50);
	Roboto_60 = TTF_OpenFont("romfs:/resources/Roboto-Regular.ttf", 60);
	Roboto_80 = TTF_OpenFont("romfs:/resources/Roboto-Regular.ttf", 80);
	Roboto_max = TTF_OpenFont("romfs:/resources/Roboto-Regular.ttf", 100);
}
