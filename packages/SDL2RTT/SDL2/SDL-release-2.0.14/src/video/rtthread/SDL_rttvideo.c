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

#include "../SDL_sysvideo.h"
#include "../../render/SDL_sysrender.h"
#include "../../events/SDL_keyboard_c.h"
#include "../../events/SDL_mouse_c.h"
#include "../../events/SDL_windowevents_c.h"

#include "SDL_rttvideo.h"
#include "SDL_rtttouch.h"
#include "SDL_rttkeyboard.h"
#include "SDL_rttmouse.h"

#define RTT_SURFACE "RTTSurface"

struct SDL_PixelFormatMap 
{
    int a; 
    SDL_PixelFormatEnum b; 
};

/* Currently only one window */
static SDL_Window *rtt_window = NULL;
static rt_device_t device;
static struct rt_device_graphic_info info; 

void *RTT_GetWindowInfo(void)
{
    return (void *)&info; 
}

/* RT-Thread SDL图像格式映射表 */ 
const static struct SDL_PixelFormatMap map[] = 
{
    // {RTGRAPHIC_PIXEL_FORMAT_MONO   , SDL_PIXELFORMAT_UNKNOWN }, 
    // {RTGRAPHIC_PIXEL_FORMAT_GRAY4  , SDL_PIXELFORMAT_UNKNOWN }, 
    // {RTGRAPHIC_PIXEL_FORMAT_GRAY16 , SDL_PIXELFORMAT_UNKNOWN }, 
    // {RTGRAPHIC_PIXEL_FORMAT_RGB332 , SDL_PIXELFORMAT_RGB332  }, 
    // {RTGRAPHIC_PIXEL_FORMAT_RGB444 , SDL_PIXELFORMAT_RGB444  }, 
    // {RTGRAPHIC_PIXEL_FORMAT_RGB565 , SDL_PIXELFORMAT_RGB565  }, 
    // {RTGRAPHIC_PIXEL_FORMAT_RGB565P, SDL_PIXELFORMAT_RGB565  }, 
    // {RTGRAPHIC_PIXEL_FORMAT_BGR565 , SDL_PIXELFORMAT_BGR565  }, 
    // {RTGRAPHIC_PIXEL_FORMAT_RGB565P, SDL_PIXELFORMAT_BGR565  }, 
    // {RTGRAPHIC_PIXEL_FORMAT_RGB666 , SDL_PIXELFORMAT_UNKNOWN }, 
    {RTGRAPHIC_PIXEL_FORMAT_RGB888 , SDL_PIXELFORMAT_BGR24   }, 
    // {RTGRAPHIC_PIXEL_FORMAT_ARGB888, SDL_PIXELFORMAT_ARGB8888}, 
    // {RTGRAPHIC_PIXEL_FORMAT_ABGR888, SDL_PIXELFORMAT_ABGR8888}, 
    // {RTGRAPHIC_PIXEL_FORMAT_ARGB565, SDL_PIXELFORMAT_UNKNOWN }, 
    // {RTGRAPHIC_PIXEL_FORMAT_ALPHA  , SDL_PIXELFORMAT_UNKNOWN }, 
    // {RTGRAPHIC_PIXEL_FORMAT_COLOR  , SDL_PIXELFORMAT_UNKNOWN }, 
}; 

static SDL_PixelFormatEnum RTT_FormatConvert(int rttformat)
{
    Uint32 format = SDL_PIXELFORMAT_UNKNOWN;
    int i;

    for(i = 0; i < sizeof(map)/sizeof(map[0]); i++)
    {
        if(rttformat == map[i].a) 
        {
            format = map[i].b; 
        }
    }

    return format; 
}

int RTT_VideoInit(_THIS)
{
    SDL_VideoDisplay display;
    SDL_DisplayMode current_mode;
    
    /* For RT-Thread */ 
    device = rt_device_find("lcd"); 
    if(!device)
    {
        return SDL_SetError("Not found RT-Thread LCD Device");
    }

    rt_device_open(device, RT_DEVICE_OFLAG_RDWR); 
    rt_device_control(device, RTGRAPHIC_CTRL_GET_INFO, &info); 

    SDL_zero(current_mode);
    current_mode.w = info.width;
    current_mode.h = info.height;
    current_mode.refresh_rate = 30; 
    current_mode.format = RTT_FormatConvert(info.pixel_format);
    current_mode.driverdata = NULL;

    SDL_zero(display);
    display.desktop_mode = current_mode;
    display.current_mode = current_mode;
    display.driverdata = NULL;
    SDL_AddVideoDisplay(&display, SDL_FALSE);

    // init touch
    RTT_InitTouch();
    // init keyboard
    RTT_InitKeyboard();
    // init mouse
    RTT_InitMouse();

    return 0;
}

void RTT_VideoQuit(_THIS)
{
    // exit touch
    RTT_QuitTouch();
    // exit keyboard
    RTT_QuitKeyboard();
    // exit mouse
    RTT_QuitMouse();
}

// #define RTT_TAG printf("######## %s %s %d\n", __FILE__, __func__, __LINE__) 
#define RTT_TAG 

void RTT_GetDisplayModes(_THIS, SDL_VideoDisplay *display)
{
    RTT_TAG; 
    SDL_DisplayMode mode;
    SDL_AddDisplayMode(display, &display->current_mode); 
}

int RTT_SetDisplayMode(_THIS, SDL_VideoDisplay *display, SDL_DisplayMode *mode)
{
    RTT_TAG; 
    return SDL_SetError("RT-Thread only supports default mode");
}

int RTT_CreateWindow(_THIS, SDL_Window *window)
{
    RTT_TAG; 
    if(rtt_window != NULL) 
    {
        return SDL_SetError("RT-Thread only supports one window");
    }

    /* Setup driver data for this window */
    window->driverdata = NULL;
    rtt_window = window; 

    /* One window, it always has focus */
    SDL_SetMouseFocus(window);
    SDL_SetKeyboardFocus(window); 

    /* Window has been successfully created */
    return 0;
}

void RTT_DestroyWindow(_THIS, SDL_Window *window)
{
    RTT_TAG; 
    if(window == rtt_window) 
    {
        rtt_window = NULL;
    }
}

int RTT_CreateWindowFrom(_THIS, SDL_Window *window, const void *data)
{
    RTT_TAG; 
    return -1;
}

void RTT_SetWindowTitle(_THIS, SDL_Window *window) {RTT_TAG; }
void RTT_SetWindowIcon(_THIS, SDL_Window *window, SDL_Surface *icon) {RTT_TAG; }
void RTT_SetWindowPosition(_THIS, SDL_Window *window) {RTT_TAG; }
void RTT_SetWindowSize(_THIS, SDL_Window *window) {RTT_TAG; }
void RTT_ShowWindow(_THIS, SDL_Window *window) {RTT_TAG; }
void RTT_HideWindow(_THIS, SDL_Window *window) {RTT_TAG; }
void RTT_RaiseWindow(_THIS, SDL_Window *window) {RTT_TAG; } 
void RTT_MaximizeWindow(_THIS, SDL_Window *window) {RTT_TAG; }
void RTT_MinimizeWindow(_THIS, SDL_Window *window) {RTT_TAG; }
void RTT_RestoreWindow(_THIS, SDL_Window *window) {RTT_TAG; }
void RTT_SetWindowGrab(_THIS, SDL_Window *window, SDL_bool grabbed) {RTT_TAG; }

void RTT_PumpEvents(_THIS)
{
    RTT_PollTouch();
    RTT_PollKeyboard();
    RTT_PollMouse();
}

int RTT_CreateWindowFramebuffer(_THIS, SDL_Window * window, Uint32 * format, void ** pixels, int *pitch)
{
    SDL_Surface *surface;
    const Uint32 surface_format = RTT_FormatConvert(info.pixel_format);
    int w, h;
    int bpp;
    Uint32 Rmask, Gmask, Bmask, Amask;

    /* Free the old framebuffer surface */
    surface = (SDL_Surface *)SDL_GetWindowData(window, RTT_SURFACE);
    SDL_FreeSurface(surface);

    /* Create a new one */
    SDL_PixelFormatEnumToMasks(surface_format, &bpp, &Rmask, &Gmask, &Bmask, &Amask);
    SDL_GetWindowSize(window, &w, &h);

    SDL_LogDebug(SDL_LOG_CATEGORY_VIDEO, "bpp   = %d\n", bpp); 
    SDL_LogDebug(SDL_LOG_CATEGORY_VIDEO, "Rmask = %d\n", Rmask); 
    SDL_LogDebug(SDL_LOG_CATEGORY_VIDEO, "Gmask = %d\n", Gmask); 
    SDL_LogDebug(SDL_LOG_CATEGORY_VIDEO, "Bmask = %d\n", Bmask); 
    SDL_LogDebug(SDL_LOG_CATEGORY_VIDEO, "Amask = %d\n", Amask); 
    SDL_LogDebug(SDL_LOG_CATEGORY_VIDEO, "witch = %d\n", w); 
    SDL_LogDebug(SDL_LOG_CATEGORY_VIDEO, "hight = %d\n", h); 

    surface = SDL_CreateRGBSurface(0, w, h, bpp, Rmask, Gmask, Bmask, Amask);
    if(!surface) 
    {
        return SDL_SetError("create RGB surface fail");
    }

    /* Save the info and return! */
    SDL_SetWindowData(window, RTT_SURFACE, surface); 
    *format = surface_format;
    *pixels = surface->pixels;
    *pitch  = surface->pitch;

    return 0;
}

int RTT_UpdateWindowFramebuffer(_THIS, SDL_Window * window, const SDL_Rect * rects, int numrects)
{
    SDL_Surface *surface;
    struct rt_device_rect_info rect; 

    surface = (SDL_Surface *)SDL_GetWindowData(window, RTT_SURFACE);
    if (!surface)
    {
        return SDL_SetError("Couldn't find RTT surface for window");
    }

    rect.x = 0;
    rect.y = 0; 
    rect.width = info.width; 
    rect.height = info.height; 

    SDL_LogDebug(SDL_LOG_CATEGORY_VIDEO, "UpdateWindowFramebuffer: w %d, h %d, bits_per_pixel %d\n", 
        info.width, info.height, info.bits_per_pixel); 

    rt_memcpy(info.framebuffer, surface->pixels, info.width * info.height * info.bits_per_pixel/8); 
    rt_device_control(device, RTGRAPHIC_CTRL_RECT_UPDATE, &rect); 

    return 0; 
}

void RTT_DestroyWindowFramebuffer(_THIS, SDL_Window * window)
{
    SDL_Surface *surface;

    surface = (SDL_Surface *)SDL_SetWindowData(window, RTT_SURFACE, NULL);
    SDL_FreeSurface(surface);
}

static void RTT_Destroy(SDL_VideoDevice *device)
{
    if (device != NULL)
    {
        if (device->driverdata != NULL)
        {
            SDL_free(device->driverdata);
        }
        SDL_free(device);
    }
}

static SDL_VideoDevice *RTT_CreateDevice(int devindex)
{
    SDL_VideoDevice *device;

    /* 初始化SDL_VideoDevice结构 */
    device = (SDL_VideoDevice *)SDL_calloc(1, sizeof(SDL_VideoDevice));
    if (device == NULL)
    {
        SDL_OutOfMemory();
        return NULL;
    }

    /* 可用显示的设置数量 */
    device->num_displays = 0;

    /* 设置 device 释放功能 */
    device->free = RTT_Destroy; 

    /* 设置所有我们可以处理的函数 */
    device->VideoInit           = RTT_VideoInit;
    device->VideoQuit           = RTT_VideoQuit;
    device->GetDisplayModes     = RTT_GetDisplayModes;
    device->SetDisplayMode      = RTT_SetDisplayMode;
    device->CreateSDLWindow     = RTT_CreateWindow;
    device->DestroyWindow       = RTT_DestroyWindow;
    device->CreateSDLWindowFrom = RTT_CreateWindowFrom;
    device->SetWindowTitle      = RTT_SetWindowTitle;
    device->SetWindowIcon       = RTT_SetWindowIcon;
    device->SetWindowPosition   = RTT_SetWindowPosition;
    device->SetWindowSize       = RTT_SetWindowSize;
    device->ShowWindow          = RTT_ShowWindow;
    device->HideWindow          = RTT_HideWindow;
    device->RaiseWindow         = RTT_RaiseWindow;
    device->MaximizeWindow      = RTT_MaximizeWindow;
    device->MinimizeWindow      = RTT_MinimizeWindow;
    device->RestoreWindow       = RTT_RestoreWindow;
    device->SetWindowGrab       = RTT_SetWindowGrab;

    /* Framebuffer */ 
    device->CreateWindowFramebuffer  = RTT_CreateWindowFramebuffer;
    device->UpdateWindowFramebuffer  = RTT_UpdateWindowFramebuffer;
    device->DestroyWindowFramebuffer = RTT_DestroyWindowFramebuffer;

    /* SDL Event */ 
    device->PumpEvents = RTT_PumpEvents;
    
    return device;
}

VideoBootStrap RTT_bootstrap = 
{
    "RT-Thread",
    "RT-Thread OS/SMART video driver",
    RTT_CreateDevice
};

#endif /* SDL_VIDEO_DRIVER_RTTHREAD */
