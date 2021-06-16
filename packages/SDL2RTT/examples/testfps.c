#include "rtthread.h"
#include "SDL.h"

int main_testfps(int argc, char *argv[])
{
    int ret = SDL_Init(SDL_INIT_VIDEO);
    if (ret != 0)
    {
        printf("SDL_Init error\n");
        return -1;
    }

    SDL_Window *pWin = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 320, 480, 0);
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

    Uint64 nFrequency, nPrevCounter, nCurrCounter, nElapsedCounter;
    float elapsed = 0.0f, totalTime = 0.0f;
    int fps = 0, fpsCount = 0;

    nFrequency = SDL_GetPerformanceFrequency();
    nPrevCounter = SDL_GetPerformanceCounter();

    srand((unsigned)rt_tick_get());

    while (!quit)
    {
        if (SDL_PollEvent(&evt))
        {
            if (evt.type == SDL_QUIT)
            {
                quit = 1;
            }
        }
        else
        {
            nCurrCounter = SDL_GetPerformanceCounter();
            nElapsedCounter = nCurrCounter - nPrevCounter;
            nPrevCounter = nCurrCounter;

            // 前后两帧的耗时(ms)
            elapsed = (nElapsedCounter * 1000.0f) / nFrequency;

            // 清除背景
            SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);
            SDL_RenderClear(pRenderer);

            // 渲染图像
            SDL_Surface *bmp = SDL_LoadBMP("/0.bmp");
            SDL_Texture *tex = SDL_CreateTextureFromSurface(pRenderer, bmp);
            SDL_RenderCopy(pRenderer, tex, NULL, NULL);
            SDL_FreeSurface(bmp);
            SDL_DestroyTexture(tex);

            // 显示图像
            SDL_RenderPresent(pRenderer);

            totalTime += elapsed;
            if (totalTime > 1000.0f) // 已经过了 1 秒
            {
                totalTime -= 1000.0f;
                fps = fpsCount;
                fpsCount = 1;
                SDL_Log("%d\n", fps);
            }
            else
            {
                fpsCount++;
            }
        }
    }

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
MSH_CMD_EXPORT_ALIAS(main_testfps, testfps, test fps.);
