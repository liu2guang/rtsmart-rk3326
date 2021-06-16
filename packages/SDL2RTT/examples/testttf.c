#include "rtthread.h"
#include "SDL.h"
#include "SDL_ttf.h"

int main_testttf(int argc, char *argv[])
{
    int ret = SDL_Init(SDL_INIT_VIDEO);
    if (ret != 0) {
        printf("SDL_Init error\n"); 
        return -1;
    }

    SDL_Window* pWin = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 320, 480, 0);
    if (pWin == NULL) {
        printf("SDL_CreateWindow error\n"); 
        return -2;
    }

    SDL_Renderer* pRenderer = SDL_CreateRenderer(pWin, -1, SDL_RENDERER_SOFTWARE);
    if (pRenderer == NULL) {
        printf("SDL_CreateRenderer error\n"); 
        SDL_DestroyWindow(pWin);
        return -3;
    }

    int quit = 0;
    SDL_Event evt;

    TTF_Init(); 

    SDL_Color color = { 255, 255, 255, 255 };
    SDL_Rect dstrect = {0, 0, 160, 50}; 
    // TTF_Font* font = TTF_OpenFont("/lazy.ttf", 24);
    TTF_Font* font = TTF_OpenFont("/dfptanliw5_b5.ttf", 24);

    // 清除背景
    SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);
    SDL_RenderClear(pRenderer);

    // 文字渲染
    // SDL_Surface* surf = TTF_RenderText_Blended(font, "are you OK ?", color);
    // SDL_Surface* surf = TTF_RenderText_Blended(font, "Double player", color);
    // SDL_Surface* surf = TTF_RenderText_Solid(font, "Double player", color);

    Uint16 msg[1024] = {0x4F60, 0x597D, 0}; 
    SDL_Surface* surf = TTF_RenderUNICODE_Blended(font, msg, color);
    if (surf == NULL)
    {
        printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
    }

    SDL_Texture* ftex = SDL_CreateTextureFromSurface(pRenderer, surf);
    SDL_RenderCopy(pRenderer, ftex, NULL, &dstrect);
    SDL_FreeSurface(surf);
    SDL_DestroyTexture(ftex);

    //显示图像
    SDL_RenderPresent(pRenderer);

    if (pRenderer) {
        SDL_DestroyRenderer(pRenderer);
    }

    if (pWin) {
        SDL_DestroyWindow(pWin);
    }
    
    TTF_Quit(); 
    SDL_Quit();

    return 0;
}
MSH_CMD_EXPORT_ALIAS(main_testttf, testttf, test ttf.);
