#include "rtthread.h"
#include "SDL.h"
#include "SDL_image.h"

int main_testimg(int argc, char *argv[])
{
    int ret = SDL_Init(SDL_INIT_VIDEO);
    if (ret != 0)
    {
        printf("SDL_Init error\n");
        return -1;
    }

    SDL_Window *pWin = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, 320, 480, 0);
    if (pWin == NULL)
    {
        printf("SDL_CreateWindow error\n");
        return -2;
    }

    SDL_Renderer *pRenderer = SDL_CreateRenderer(pWin, -1, SDL_RENDERER_SOFTWARE);
    if (pRenderer == NULL)
    {
        printf("SDL_CreateRenderer error\n");
        SDL_DestroyWindow(pWin);
        return -3;
    }

    int quit = 0;
    SDL_Event evt;

    // 清除背景
    SDL_SetRenderDrawColor(pRenderer, 0xFF, 0x00, 0x00, 0xFF);
    SDL_RenderClear(pRenderer);

    SDL_Delay(1000);

    // 渲染图像
    // SDL_Surface *bmp = SDL_LoadBMP("/0.bmp");
    // SDL_Surface *bmp = IMG_Load("/sd/222.png");
    SDL_Surface *bmp = IMG_Load(argv[1]?argv[1]:"/0.bmp");
    SDL_Texture *tex = SDL_CreateTextureFromSurface(pRenderer, bmp);
    SDL_RenderCopy(pRenderer, tex, NULL, NULL);
    SDL_FreeSurface(bmp);
    SDL_DestroyTexture(tex);

    //显示图像
    SDL_RenderPresent(pRenderer);

    if (pRenderer)
    {
        SDL_DestroyRenderer(pRenderer);
    }

    if (pWin)
    {
        SDL_DestroyWindow(pWin);
    }

    SDL_Quit();

    return 0;
}
MSH_CMD_EXPORT_ALIAS(main_testimg, testimg, test img.);
