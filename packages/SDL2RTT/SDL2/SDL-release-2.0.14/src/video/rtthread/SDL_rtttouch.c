/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2017 Sam Lantinga <slouken@libsdl.org>

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

#if SDL_VIDEO_DRIVER_RTTHREAD && defined(RT_USING_TOUCH)

#include <rtthread.h>
#include <touch.h>

#include "SDL_events.h"
#include "SDL_hints.h"
#include "SDL_rttvideo.h"
#include "../../events/SDL_touch_c.h"
#include "../../video/SDL_sysvideo.h"

static struct rt_touch_device *touch = NULL; 
static struct rt_touch_data touch_data;
static struct rt_device_graphic_info *info = NULL; 

void RTT_InitTouch(void)
{
    touch = (struct rt_touch_device *)rt_device_find("touch"); 
    if(touch)
    {
        rt_device_open(&(touch->parent), RT_DEVICE_FLAG_RDONLY);
        
        SDL_LogDebug(SDL_LOG_CATEGORY_INPUT, "Touch Type    : %d", touch->info.type); 
        SDL_LogDebug(SDL_LOG_CATEGORY_INPUT, "Touch Vendor  : %d", touch->info.vendor); 
        SDL_LogDebug(SDL_LOG_CATEGORY_INPUT, "Touch PointNum: %d", touch->info.point_num); 
        SDL_LogDebug(SDL_LOG_CATEGORY_INPUT, "Touch RangeX  : %d", touch->info.range_x); 
        SDL_LogDebug(SDL_LOG_CATEGORY_INPUT, "Touch RangeY  : %d", touch->info.range_y); 
    
        info = (struct rt_device_graphic_info *)RTT_GetWindowInfo(); 

        SDL_AddTouch((SDL_TouchID) 0, SDL_TOUCH_DEVICE_DIRECT, "RTTTouch");
        SDL_SetHintWithPriority(SDL_HINT_TOUCH_MOUSE_EVENTS, "0", SDL_HINT_DEFAULT);
    } 
}

void RTT_QuitTouch(void)
{
    if(touch)
    {
        info = NULL; 
        rt_device_close(touch); 
        touch = NULL; 
    }
}

void RTT_PollTouch(void)
{
    if(touch)
    {
        SDL_Window *window = SDL_GetFocusWindow();
        if (window == NULL)
        {
            return;
        }

        rt_device_read(&(touch->parent), 0, &touch_data, 1); 
        if(touch_data.event == RT_TOUCH_EVENT_DOWN)
        {
            SDL_SendTouch(0, (SDL_FingerID) 0, window, SDL_TRUE,
                (float)touch_data.x_coordinate / (float)(info->width),
                (float)touch_data.y_coordinate / (float)(info->height), 1); 
        }
        else if(touch_data.event == RT_TOUCH_EVENT_UP)
        {
            SDL_SendTouch(0, (SDL_FingerID) 0, window, SDL_FALSE,
                (float)touch_data.x_coordinate / (float)(info->width),
                (float)touch_data.y_coordinate / (float)(info->height), 1);
        }
        else if(touch_data.event == RT_TOUCH_EVENT_MOVE)
        {
            SDL_SendTouchMotion(0, (SDL_FingerID) 0, window,
                (float)touch_data.x_coordinate / (float)(info->width),
                (float)touch_data.y_coordinate / (float)(info->height), 1);
        }
    }
}

#else 

void RTT_InitTouch(void){}
void RTT_QuitTouch(void){}
void RTT_PollTouch(void){}

#endif /* SDL_VIDEO_DRIVER_RTTHREAD */

/* vi: set ts=4 sw=4 expandtab: */
