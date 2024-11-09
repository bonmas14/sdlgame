#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include <stdio.h>
#include <time.h>
#include "SDL.h"

#define LOGFILE_NAME "log"
#define WIDTH        640
#define HEIGHT       480

FILE *logfile = NULL;

void print_error(const char *message) { 
    char buffer[128];
    time_t curr     = time(NULL);
    struct tm * utc = gmtime(&curr);

    (void)strftime(buffer, 128, "UTC: %H:%M:%S %d/%m/%y", utc);

    fprintf(logfile, "[ERROR (%s)]: %s\n", buffer, message);
}


SDL_Window*   window   = NULL;
SDL_Renderer* renderer = NULL;


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
    logfile = fopen(LOGFILE_NAME, "ab");
    if (!logfile) {
        return -100;
    }

    print_error("Test error message.");


	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        print_error("Error while initializing SDL.");
        return -1;
    }

	window   = SDL_CreateWindow("I hate firefox", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);

    if (!window) {
        print_error("Error while creating window.");
        return -1;
    }

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!renderer) {
        print_error("Error while creating renderer.");
        return -1;
    }

    while (loop()) { }

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
    fclose(logfile);
	return 0;
}
