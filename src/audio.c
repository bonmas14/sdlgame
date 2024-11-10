#include "audio.h"

SDL_SpinLock status_lock = { 0 };
bool exit_request = 0;

// AudioThreadSpecific

SDL_AudioDeviceID current_device_id    = { 0 };
SDL_AudioSpec     current_audio_config = { 0 };

void audio_system_request_exit(void) {
    while (!SDL_AtomicTryLock(&status_lock)) {
        // waiting for lock
    }

    exit_request = true;

    SDL_AtomicUnlock(&status_lock);
}

bool is_audio_exit_requested(void) {
    bool buff;

    while (!SDL_AtomicTryLock(&status_lock)) {
        // waiting for lock
    }

    buff = exit_request;

    SDL_AtomicUnlock(&status_lock);

    return buff;
}

int init_audio_system(void) {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Couldn't init audio subsystem.");
        return -110;
    }

    SDL_AudioSpec audio_requested = { 0 };
    audio_requested.format        = AUDIO_F32;
    audio_requested.channels      = 2;
    audio_requested.freq          = 44100;

    current_device_id = SDL_OpenAudioDevice(NULL, false, &audio_requested, &current_audio_config, 0);

    if (!current_device_id) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Couldn't open audio device.");
        return -111;
    }

    return 0;
}

int start_audio_system(void * data) {
    SDL_Log("Started audio thread.");

    int code = init_audio_system();

    if (code < 0) {
        if (code == -111) SDL_QuitSubSystem(SDL_INIT_AUDIO);
        SDL_Log("Stopped thread.");
        return code;
    }

    while (!is_audio_exit_requested()) {
        // audio update / mixer loop 
    }

    SDL_CloseAudioDevice(current_device_id);
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    SDL_Log("Stopped thread.");
    return 0;
}
