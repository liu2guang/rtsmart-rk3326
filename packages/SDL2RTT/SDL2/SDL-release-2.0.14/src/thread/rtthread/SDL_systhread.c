/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2020 Sam Lantinga <slouken@libsdl.org>

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

/* Thread management routines for SDL */

#include "SDL_thread.h"
#include "../SDL_systhread.h"

// #define STACK_SIZE 0x8000
#define STACK_SIZE 0x20000

static void SDL_SYS_RunThread(void *p)
{
    SDL_RunThread((SDL_Thread *)p);
}

int SDL_SYS_CreateThread(SDL_Thread *thread)
{
    int priority = 20;

    thread->handle = rt_thread_create(thread->name, SDL_SYS_RunThread, 
        (void *)thread, thread->stacksize ? ((int) thread->stacksize) : STACK_SIZE, 
        priority, 10
    ); 

    if(!thread->handle) 
    {
        return SDL_SetError("SDL_SYS_CreateThread failed");
    }

    rt_thread_startup(thread->handle); 

    return 0;
}

void SDL_SYS_SetupThread(const char *name)
{
    return;
}

SDL_threadID SDL_ThreadID(void)
{
    return (SDL_threadID)rt_thread_self();
}

int SDL_SYS_SetThreadPriority(SDL_ThreadPriority priority)
{
    int priority_num;

    switch(priority)
    {
    case SDL_THREAD_PRIORITY_LOW:
        priority_num = 25;
        break; 
    case SDL_THREAD_PRIORITY_NORMAL:
        priority_num = 20;
        break; 
    case SDL_THREAD_PRIORITY_HIGH:
        priority_num = 15;
        break; 
    case SDL_THREAD_PRIORITY_TIME_CRITICAL:
        priority_num = 10;
        break; 
    }

    return rt_thread_control(rt_thread_self(), RT_THREAD_CTRL_CHANGE_PRIORITY, &priority_num); 
}

void SDL_SYS_WaitThread(SDL_Thread *thread)
{
    if(!thread || !thread->handle) 
    {
        return SDL_SetError("SDL_SYS_WaitThread failed");
    }

    while(1)
    {
        rt_thread_t tid = rt_thread_find(thread->name); 
        if(!tid)
        {
            break; 
        }

        rt_thread_mdelay(1); 
    }
}

void SDL_SYS_DetachThread(SDL_Thread *thread)
{
    if(!thread || !thread->handle) 
    {
        return SDL_SetError("SDL_SYS_DetachThread failed");
    }

    rt_thread_delete(thread->handle); 
}

/* vi: set ts=4 sw=4 expandtab: */
