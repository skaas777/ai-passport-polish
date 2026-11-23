#pragma once

#include <stdbool.h>
#include <stddef.h>

// Initialize once at boot. Reads below are RAM-only; writes belong in the input task.
void polish_progress_init(void);
size_t polish_progress_last_topic(void);
bool polish_progress_completed(size_t topic);
void polish_progress_opened(size_t topic);
void polish_progress_mastered(size_t topic);
