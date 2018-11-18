#ifndef SDL_HELPER_H
#define SDL_HELPER_H
#include <switch.h>

SDL_Window * 	window;
SDL_Renderer * 	renderer;
SDL_Surface *	surface;
SDL_Texture *	texture;

typedef struct 
{
	SDL_Texture * texture;
	SDL_Rect SrcR;
	SDL_Rect DestR;
} 
images;
images icon, iconframe, background[2];
 
static inline SDL_Color SDL_MakeColour(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	SDL_Color colour = {r, g, b, a};
	return colour;
}


#define WHITE                 SDL_MakeColour(255, 255, 255, 255)
#define RED		              SDL_MakeColour(255, 0, 0, 255)
#define RED_DARK	          SDL_MakeColour(102, 0, 0, 255)
#define GRAY 				  SDL_MakeColour(192, 192, 192, 255)
#define GRAY_LIGHT			  SDL_MakeColour(222, 222, 222, 255)
#define GREEN				  SDL_MakeColour(0, 255, 0, 255)
#define BLACK_BG              SDL_MakeColour(48, 48, 48, 255)
#define BLACK                 SDL_MakeColour(0, 0, 0, 255)


void SDL_ClearScreen(SDL_Renderer *renderer, SDL_Color colour);
void SDL_LoadImage(SDL_Texture** texture, u8* buff, size_t size);
void SDL_DrawImageScale(SDL_Texture *texture, int x, int y, int w, int h);
void SDL_DrawRect(SDL_Renderer *renderer, int x, int y, int w, int h, SDL_Color colour);
void SDL_DrawCircle(SDL_Renderer *renderer, int x, int y, int r, SDL_Color colour);
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int Srcx, int Srcy, int Destx, int Desty, int w, int h);
void SDL_DrawText(SDL_Renderer *renderer, TTF_Font *font, int x, int y, SDL_Color colour, const char *text);
#endif
