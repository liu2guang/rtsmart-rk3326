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

/* An implementation of semaphores using mutexes and condition variables */

#include "SDL_timer.h"
#include "SDL_thread.h"
#include "SDL_systhread_c.h"

struct SDL_semaphore 
{
    rt_sem_t sem;
};

SDL_sem *SDL_CreateSemaphore(Uint32 initial_value)
{
    SDL_sem *sem;

    sem = (SDL_sem *)SDL_malloc(sizeof(*sem));
    if(sem != NULL)
    {
        sem->sem = rt_sem_create("SDLSem", initial_value, RT_IPC_FLAG_FIFO); 
        if (!(sem->sem))
        {
            SDL_SetError("Couldn't create semaphore");
            SDL_free(sem);
            sem = NULL;
        }
    }
    else
    {
        SDL_OutOfMemory();
    }

    return sem;
}

void SDL_DestroySemaphore(SDL_sem * sem)
{
    if(sem != NULL)
    {
        if(sem->sem)
        {
            rt_sem_delete(sem->sem);
            sem->sem = NULL; 
        }
        SDL_free(sem); 
    }
}

int SDL_SemWaitTimeout(SDL_sem *sem, Uint32 timeout)
{
    rt_int32_t timeout_rtt;
    rt_err_t ret;

    if(sem == NULL) 
    {
        SDL_SetError("Passed a NULL sem");
        return 0;
    }

    if(timeout == 0) 
    {
        ret = rt_sem_trytake(sem->sem);
        if (ret == -RT_ETIMEOUT) 
        {
            return SDL_MUTEX_TIMEDOUT;
        }
        return 0; 
    }

    if(timeout == SDL_MUTEX_MAXWAIT) 
    {
        timeout_rtt = RT_WAITING_FOREVER;
    } 
    else 
    {
        /* Convert to microseconds. */  
        // timeout_rtt = timeout * 1000; 
        timeout_rtt = timeout; 
    }

    ret = rt_sem_take(sem->sem, timeout_rtt);
    switch(ret) 
    {
    case RT_EOK:
        return 0;
    case -RT_ETIMEOUT:
        return SDL_MUTEX_TIMEDOUT;
    default:
        return SDL_SetError("sceKernelWaitSema() failed");
    }
}

int SDL_SemTryWait(SDL_sem *sem)
{
    return SDL_SemWaitTimeout(sem, 0);
}

int SDL_SemWait(SDL_sem *sem)
{
    return SDL_SemWaitTimeout(sem, SDL_MUTEX_MAXWAIT);
}

Uint32 SDL_SemValue(SDL_sem *sem)
{
    if(sem == NULL) 
    {
        SDL_SetError("Passed a NULL sem");
        return 0;
    }

    return (int)(sem->sem->value); 
}

int SDL_SemPost(SDL_sem *sem)
{
    rt_err_t ret;

    if(sem == NULL) 
    {
        return SDL_SetError("Passed a NULL sem");
    }

    ret = rt_sem_release(sem->sem);
    if (ret != RT_EOK) 
    {
        return SDL_SetError("rt_sem_release failed");
    }

    return 0;
}

/* vi: set ts=4 sw=4 expandtab: */
