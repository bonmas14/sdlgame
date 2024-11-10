#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include <stdio.h>

#include "audio.h"
#include "input.h"

#include <SDL.h>
#include <SDL_thread.h>
#include <SDL_atomic.h>

#define WIDTH        640
#define HEIGHT       480

SDL_Window   *window   = NULL;
SDL_Renderer *renderer = NULL;




void draw(void) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawLineF(renderer, 0, 0, WIDTH, HEIGHT);

    SDL_RenderPresent(renderer);
}

bool loop(void) {
    SDL_Event event;

    if (SDL_WaitEvent(&event)) {
        if (event.type == SDL_QUIT) {
            return false;
        }
    }

    draw();

    return true;
}

int main(int argc, char* argv[]) {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error while initializing SDL.");
        return -1;
    }

	window   = SDL_CreateWindow("I hate firefox", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);

    if (!window) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error while creating window.");
        return -1;
    }

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!renderer) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error while creating renderer.");
        SDL_DestroyWindow(window);
        return -1;
    }


    SDL_Thread *audio_thread = SDL_CreateThread((SDL_ThreadFunction)start_audio_system, "AudioSystem", NULL);

    if (!audio_thread) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error while creating audio thread.");
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    while (loop()) { }

    int status = 0;
    audio_system_request_exit();
    SDL_WaitThread(audio_thread, &status);

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
