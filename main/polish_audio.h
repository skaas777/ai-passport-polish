// main/polish_audio.h -- Audio playback for Polish learning
#pragma once

#include "esp_err.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// Initialize audio playback system
esp_err_t polish_audio_init(void);

// Play audio for a specific card index
// This will play the pre-recorded Polish pronunciation
esp_err_t polish_audio_play(int card_index);

// Stop current playback
esp_err_t polish_audio_stop(void);

// Check if audio is currently playing
bool polish_audio_is_playing(void);

// Deinitialize audio system
esp_err_t polish_audio_deinit(void);
