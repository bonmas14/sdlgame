#pragma once 

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include <SDL.h>
#include <SDL_audio.h>
#include <SDL_atomic.h>

int  start_audio_system(void *data);
void audio_system_request_exit(void);
