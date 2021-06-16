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

#if SDL_AUDIO_DRIVER_RTTHREAD

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>

#include "SDL_audio.h"
#include "../SDL_audio_c.h"
#include "../SDL_audiodev_c.h"

#include "SDL_rttaudio.h"

#include <rtthread.h>
#include <rtdevice.h>

static rt_device_t device; 

static int RTTAUDIO_OpenDevice(_THIS, void *handle, const char *devname, int iscapture)
{
    int format, mixlen, i;

    /* 检测是否是录音设备 */ 
    if(iscapture)
    {
        SDL_SetError("Couldn't open capture device: %s", devname);
    }

    this->hidden = (struct SDL_PrivateAudioData *)SDL_malloc(sizeof(*this->hidden));
    if(this->hidden == NULL) 
    {
        return SDL_OutOfMemory();
    }
    SDL_zerop(this->hidden);

    switch (this->spec.format & 0xff) 
    {
    case 16:
        this->spec.format = AUDIO_S16LSB;
        break;
    default:
        return SDL_SetError("Unsupported audio format");
    }
    
    /* The sample count must be a multiple of 64. */
    this->spec.freq = 44100;

    /* Update the fragment size as size in bytes. */
    SDL_CalculateAudioSpec(&this->spec);

    /* Allocate the mixing buffer.  Its size and starting address must
       be a multiple of 64 bytes.  Our sample count is already a multiple of
       64, so spec->size should be a multiple of 64 as well. */
    mixlen = this->spec.size * NUM_BUFFERS;
    this->hidden->rawbuf = (Uint8 *)rt_malloc_align(mixlen, 64);
    if (this->hidden->rawbuf == NULL) 
    {
        return SDL_SetError("Couldn't allocate mixing buffer");
    }

    /* Setup the hardware channel. */
    if (this->spec.channels == 1) 
    {
        // format = PSP_AUDIO_FORMAT_MONO;
    } 
    else 
    {
        this->spec.channels = 2;
        // format = PSP_AUDIO_FORMAT_STEREO;
    }

    memset(this->hidden->rawbuf, 0, mixlen);
    for (i = 0; i < NUM_BUFFERS; i++) 
    {
        this->hidden->mixbufs[i] = &this->hidden->rawbuf[i * this->spec.size];
    }

    this->hidden->next_buffer = 0;

    printf("this->spec.format   = %d\n", this->spec.format); 
    printf("this->spec.channels = %d\n", this->spec.channels); 
    printf("this->spec.freq     = %d\n", this->spec.freq); 
    printf("this->spec.samples  = %d\n", this->spec.samples); 
    printf("this->spec.size     = %d\n", this->spec.size); 
    printf("devname             = %s\n", devname); 

    if(devname)
    {
        this->hidden->device = rt_device_find(devname); 
    }
    else
    {
        this->hidden->device = rt_device_find("sound0"); 
    }
    rt_device_open(this->hidden->device, RT_DEVICE_OFLAG_WRONLY); 

    struct rt_audio_caps caps; 

    caps.main_type               = AUDIO_TYPE_OUTPUT; /* 输出类型（播放设备 ）*/
    caps.sub_type                = AUDIO_DSP_PARAM;   /* 设置所有音频参数信息 */
    caps.udata.config.samplerate = 44100;             /* 采样率 */
    caps.udata.config.channels   = 2;                 /* 采样通道 */
    caps.udata.config.samplebits = 16;                /* 采样位数 */
    rt_device_control(this->hidden->device, AUDIO_CTL_CONFIGURE, &caps); 

    return 0; 
}

static void RTTAUDIO_PlayDevice(_THIS)
{
    Uint8 *mixbuf = this->hidden->mixbufs[this->hidden->next_buffer];

    rt_device_write(this->hidden->device, -1, mixbuf, this->spec.size); 

    this->hidden->next_buffer = (this->hidden->next_buffer + 1) % NUM_BUFFERS;
}

static void RTTAUDIO_WaitDevice(_THIS)
{
    // printf("##### RTTAUDIO_WaitDevice\n"); 
}

static Uint8 *RTTAUDIO_GetDeviceBuf(_THIS)
{
    return this->hidden->mixbufs[this->hidden->next_buffer];
}

static void RTTAUDIO_CloseDevice(_THIS)
{
    if (this->hidden->device != NULL) 
    {
        rt_device_close(this->hidden->device); 
    }

    free(this->hidden->rawbuf);  /* this uses memalign(), not SDL_malloc(). */
    SDL_free(this->hidden);
}

static void RTTAUDIO_ThreadInit(_THIS)
{
    // printf("##### PSPAUDIO_ThreadInit\n"); 
}

static int RTTAUDIO_Init(SDL_AudioDriverImpl *impl)
{
    impl->OpenDevice   = RTTAUDIO_OpenDevice;
    impl->PlayDevice   = RTTAUDIO_PlayDevice;
    impl->WaitDevice   = RTTAUDIO_WaitDevice;
    impl->GetDeviceBuf = RTTAUDIO_GetDeviceBuf;
    impl->CloseDevice  = RTTAUDIO_CloseDevice;
    impl->ThreadInit   = RTTAUDIO_ThreadInit;

    printf("RTTAUDIO_Init\n"); 

    impl->OnlyHasDefaultOutputDevice = 1;
    impl->OnlyHasDefaultCaptureDevice = 0;
    impl->HasCaptureSupport = SDL_FALSE;

    return 1;
}

AudioBootStrap RTTAUDIO_bootstrap = {
    "rtthread", "RT-Thread OS/SMART audio driver", RTTAUDIO_Init, 0
};

#endif /* SDL_AUDIO_DRIVER_RTTHREAD */
