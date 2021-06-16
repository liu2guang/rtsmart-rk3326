#include "rtthread.h"
#include "SDL.h"
#include "SDL_audio.h"

static Uint8 *pAudio_chunk;
static Uint32 audio_len;
static Uint8 *pAudio_pos;

void fill_audio_buffer(void *userdata, Uint8 * stream, int len)
{
	SDL_memset(stream, 0, len);
	// 判断是否有读到数据
	if (audio_len == 0)
		return;

	len = (len > audio_len ? audio_len : len);
	SDL_MixAudio(stream, pAudio_pos, len, SDL_MIX_MAXVOLUME);
	pAudio_pos += len;
	audio_len -= len;
}

int playaudio(int argc, char *argv[])
{
	/*** 初始化SDL ***/
	if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER))
	{
		printf("Initialize SDL failed: %s\n", SDL_GetError());
		return -1;
	}
	/****************/

	/*** 初始化初始化SDL_AudioSpec结构体 ***/
	SDL_AudioSpec audioSpec;
	
	// 音频数据的采样率。常用的有48000，44100等
	audioSpec.freq = 44100; 
	
	// 音频数据的格式
	audioSpec.format = AUDIO_S16SYS;
	
	// 声道数。例如单声道取值为1，立体声取值为2
	audioSpec.channels = 2;
	
	// 设置静音的值
	audioSpec.silence = 0;

	// 音频缓冲区中的采样个数，要求必须是2的n次方
	audioSpec.samples = 1024;

	// 填充音频缓冲区的回调函数
	audioSpec.callback = fill_audio_buffer;
	/************************************/

	// 打开音频设备
	if (SDL_OpenAudio(&audioSpec, NULL) < 0)
	{
		printf("Can not open audio!");
		return -1;
	}

	FILE *pAudioFile = fopen("/sdcard/1.wav", "rb+");
	if (pAudioFile == NULL)
	{
		printf("Can not open audio file!");
		return -1;
	}

	int pcm_buffer_size = 4096;
	char *pcm_buffer = (char *)malloc(pcm_buffer_size);
	int data_count = 0;

	SDL_PauseAudio(0);

	for (;;)
	{
		// 循环播放
		if (fread(pcm_buffer, 1, pcm_buffer_size, pAudioFile) != pcm_buffer_size)
		{
			fseek(pAudioFile, 0, SEEK_SET);
			fread(pcm_buffer, 1, pcm_buffer_size, pAudioFile);
			data_count = 0;
		}
		// printf("Playing %10d Bytes data.\n", data_count);
		data_count += pcm_buffer_size;

		pAudio_chunk = (Uint8 *)pcm_buffer;
		audio_len = pcm_buffer_size;
		pAudio_pos = pAudio_chunk;
		
		while (audio_len > 0)
			SDL_Delay(1);
	}
	free(pcm_buffer);
	SDL_Quit();
	
	return 0;
}
MSH_CMD_EXPORT_ALIAS(playaudio, playaudio, play audio.);
