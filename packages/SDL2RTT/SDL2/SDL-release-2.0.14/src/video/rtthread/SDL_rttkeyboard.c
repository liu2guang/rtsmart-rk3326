/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2018 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
#include "../../SDL_internal.h"

#if SDL_VIDEO_DRIVER_RTTHREAD 

#include <rtthread.h> 

#include "SDL_events.h"
#include "SDL_log.h"
#include "SDL_rttvideo.h"
#include "SDL_rttkeyboard.h"
#include "../../events/SDL_keyboard_c.h"

#define NUM_SCANCODES_RTT 68 
static SDL_bool keystate[NUM_SCANCODES_RTT] = {0};

void RTT_InitKeyboard(void)
{

}

__attribute__((weak)) int RTT_HeldKeyboardCode(int index)
{
    return 0; 
}

__attribute__((weak)) int RTT_GetKeyboardCode(int keycode) 
{
    return 0; 
}

void RTT_PollKeyboard(void)
{
    // We skip polling keyboard if no window is created
    if (SDL_GetFocusWindow() == NULL)
    {
        return;
    }

    for (int i = 0; i < NUM_SCANCODES_RTT; i++)
    {
        int press = RTT_HeldKeyboardCode(i); 
        int keyCode = RTT_GetKeyboardCode(i); 

        if(press && !keystate[i])
        {
            SDL_LogInfo(SDL_LOG_CATEGORY_INPUT, "keyCode = %d, Down\n", keyCode); 
            SDL_SendKeyboardKey(SDL_PRESSED, keyCode); 
            keystate[i] = SDL_TRUE;
        }
        else if(!press && keystate[i]) 
        {
            SDL_LogInfo(SDL_LOG_CATEGORY_INPUT, "keyCode = %d, Up\n", keyCode); 
            SDL_SendKeyboardKey(SDL_RELEASED, keyCode);
            keystate[i] = SDL_FALSE;
        }
    }
}

void RTT_QuitKeyboard(void)
{

}

#endif /* SDL_VIDEO_DRIVER_RTTHREAD */

/* vi: set ts=4 sw=4 expandtab: */
