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

/* An implementation of mutexes using semaphores */

#include "SDL_thread.h"
#include "SDL_systhread_c.h"

struct SDL_mutex
{
    rt_mutex_t mutex; 
};

SDL_mutex *SDL_CreateMutex(void)
{
    SDL_mutex *mutex;

    mutex = (SDL_mutex *)SDL_malloc(sizeof(*mutex));
    if(mutex) 
    {
        mutex->mutex = rt_mutex_create("SDLMutex", RT_IPC_FLAG_FIFO);
        if(!(mutex->mutex)) 
        {
            SDL_SetError("Couldn't create mutex");
            SDL_free(mutex);
            mutex = NULL;
        }
    } 
    else 
    {
        SDL_OutOfMemory();
    }

    return mutex;
}

void SDL_DestroyMutex(SDL_mutex *mutex)
{
    if(mutex) 
    {
        if(mutex->mutex) 
        {
            rt_mutex_delete(mutex->mutex);
        }
        SDL_free(mutex);
    }
}

int SDL_LockMutex(SDL_mutex *mutex)
{
    if(mutex == NULL) 
    {
        return SDL_SetError("Passed a NULL mutex");
    }

    return rt_mutex_take(mutex->mutex, RT_WAITING_FOREVER);
}

int SDL_TryLockMutex(SDL_mutex *mutex)
{
    rt_err_t ret; 

    if(mutex == NULL) 
    {
        return SDL_SetError("Passed a NULL mutex");
    }

    ret = rt_mutex_take(mutex->mutex, 0);
    if(ret == -RT_ETIMEOUT)
    {
        return SDL_MUTEX_TIMEDOUT; 
    }

    return 0;
}

/* Unlock the mutex */
int SDL_mutexV(SDL_mutex * mutex)
{
    if(mutex == NULL) 
    {
        return SDL_SetError("Passed a NULL mutex");
    }

    return rt_mutex_release(mutex->mutex);
}

/* vi: set ts=4 sw=4 expandtab: */
