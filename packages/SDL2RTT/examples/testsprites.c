#include "rtthread.h"
#include "SDL.h"
#include "SDL_image.h"

int main_testsprites(int argc, char *argv[])
{
    SDL_Window *win = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Texture *bitmapTex = NULL;
    SDL_Surface *bitmapSurface = NULL;
    int posX = 0, posY = 0, width = 320, height = 480;
    int index = 0, index_x = 0, index_y = 0; 
    int current = 0; 

    SDL_Init(SDL_INIT_VIDEO);

    win = SDL_CreateWindow("Hello World", posX, posY, width, height, 0);

    renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_SOFTWARE);

    // bitmapSurface = IMG_Load("/sd/Gun2.png");
    bitmapSurface = IMG_Load(argc != 2 ? "/Gun2.png": argv[1]);
    bitmapTex = SDL_CreateTextureFromSurface(renderer, bitmapSurface);
    SDL_FreeSurface(bitmapSurface);

    SDL_Rect m_sourceRectangle = {0, 0, 192, 192}; 
    SDL_Rect m_destinationRectangle = {0, 0, 192, 192}; 

    current = SDL_GetTicks(); 

    while (1)
    {
        SDL_Event e;
        if (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                break;
            }
        }

        index = (int)((((SDL_GetTicks()-current)/100)%25)); 
        index_x = index / 5; 
        index_y = index % 5; 

        m_sourceRectangle.x = index_x * 192; 
        m_sourceRectangle.y = index_y * 192; 

        SDL_RenderClear(renderer);
        SDL_RenderCopyEx(renderer, bitmapTex, &m_sourceRectangle, &m_destinationRectangle, 0.0, NULL, SDL_FLIP_HORIZONTAL); 
        SDL_RenderPresent(renderer);

        if(index == 24)
        {
            break; 
        }
    }

    SDL_DestroyTexture(bitmapTex);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);

    SDL_Quit();

    return 0;
}
MSH_CMD_EXPORT_ALIAS(main_testsprites, testsprites, test sprites.);
