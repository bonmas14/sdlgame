#include "audio.h"

SDL_SpinLock status_lock = { 0 };
bool exit_request = 0;

// AudioThreadSpecific
float    *audio_buffer = NULL;
uint64_t  sample_index = 0;

SDL_AudioDeviceID current_device_id    = { 0 };
SDL_AudioSpec     current_audio_config = { 0 };

float lerp(float a, float b, float t)
{
    return a + t * (b - a);
}

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

    audio_buffer = (float*)SDL_calloc(current_audio_config.size, 1);
    if (!audio_buffer) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error allocating audio buffer.");
        return -80;
    }

    return 0;
}

int start_audio_system(void * data) {
    SDL_Log("Started audio thread.");

    int code = init_audio_system();

    if (code < 0) {
        if (code == -111) SDL_QuitSubSystem(SDL_INIT_AUDIO);
        SDL_Log("Stopped audio thread.");
        return code;
    }

    double digital_clip_level = 0.5;
    SDL_PauseAudioDevice(current_device_id, 0);
    while (!is_audio_exit_requested()) {
        Uint32 queued = SDL_GetQueuedAudioSize(current_device_id);

        // we will ahead but it just in case of very bad scheduling at SDL_Delay
        if (queued > current_audio_config.size / 4) {
            SDL_Delay(10);
            continue;
		}

		for (size_t i = 0; i < current_audio_config.samples; i++) {
			for (size_t chan = 0; chan < current_audio_config.channels; chan++) {
				// for now sample_index is global but it shouldn't be later
				double time = ((double)sample_index / (double)current_audio_config.freq);

				double amp = SDL_sin(time * 0.2) + SDL_cos(M_PI * time * 4) / 2.0 + 1.0;
				double gen3 = 0.6 * SDL_sin(M_PI * time * 9.375);
				double gen2 = 5.0 * SDL_sin(M_PI * time * 75.0 + gen3);
				double gen1 = SDL_sin(M_PI * time * 150.0 + gen2 + gen3);

				double sample = amp * gen1;

				if (sample > digital_clip_level) {
					audio_buffer[i * 2 + chan] = (float)digital_clip_level;
				}
				else if (sample < -digital_clip_level) {
					audio_buffer[i * 2 + chan] = -(float)digital_clip_level;
				}
				else {
					audio_buffer[i * 2 + chan] = (float)sample;
				}
			}
			sample_index++;
		}

		SDL_QueueAudio(current_device_id, audio_buffer, current_audio_config.size);
    }

    SDL_CloseAudioDevice(current_device_id);
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    SDL_free(audio_buffer);
    SDL_Log("Stopped audio thread.");
    return 0;
}
