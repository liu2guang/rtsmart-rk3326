#include "rtthread.h"
#include "SDL.h"
#include "SDL_image.h"

int main_testpngmixer(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
 
	SDL_Window *Screen = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, 480, 272, 0);
	SDL_Renderer *render = SDL_CreateRenderer(Screen, -1, SDL_RENDERER_SOFTWARE); 
	SDL_Surface *bk = IMG_Load("/sd/b.png"); 
	SDL_Surface *foo = IMG_Load("/sd/a.png"); 

    // 抠图, png背景是透明的所以不用扣
	Uint32 colorkey = SDL_MapRGB(foo->format, 0x00, 0xFF, 0xFF); 
	SDL_SetColorKey(foo, 1, colorkey); 
 
	SDL_Texture *texture = SDL_CreateTextureFromSurface(render, bk); 
	SDL_Texture *texture1 = SDL_CreateTextureFromSurface(render, foo); 
 
	SDL_RenderClear(render);
	SDL_RenderCopy(render, texture, NULL, NULL); 

	SDL_Rect rect;
	rect.x = 50;
	rect.y = 20;
	rect.w = foo->w;
	rect.h = foo->h;
	SDL_RenderCopy(render, texture1, NULL, &rect); 
	SDL_RenderPresent(render);
 
	SDL_FreeSurface(bk); 
    SDL_FreeSurface(foo); 
	SDL_DestroyTexture(texture); 
	SDL_DestroyRenderer(render); 
	SDL_DestroyWindow(Screen); 
	SDL_Quit(); 

	return 0;
}
MSH_CMD_EXPORT_ALIAS(main_testpngmixer, testpngmixer, test png mixer.);
