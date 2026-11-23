#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "polish_course.h"

typedef enum {
    POLISH_PAGE_TOPICS,
    POLISH_PAGE_MODES,
    POLISH_PAGE_STUDY,
    POLISH_PAGE_QUESTION,
    POLISH_PAGE_ANSWER,
    POLISH_PAGE_RESULTS,
} polish_page_t;

typedef enum {
    POLISH_INPUT_UP,
    POLISH_INPUT_DOWN,
    POLISH_INPUT_OK,
    POLISH_INPUT_BACK,
} polish_input_t;

typedef enum {
    POLISH_ACTION_NONE,
    POLISH_ACTION_REDRAW,
    POLISH_ACTION_PLAY,  // Refresh the display and pronounce the visible Polish.
} polish_action_t;

typedef struct {
    polish_page_t page;
    size_t topic;          // Zero-based course topic.
    bool quiz_selected;
    size_t position;       // Zero-based card position; equals round_size on results.
    size_t round_size;
    size_t missed;         // Cards marked for another try in the current round.
    uint8_t deck[POLISH_TOPIC_CAPACITY];
    uint32_t shuffle_state;
} polish_study_t;

void polish_study_init(polish_study_t *study, uint32_t seed);
polish_action_t polish_study_input(polish_study_t *study, polish_input_t key);

// Returns SIZE_MAX on the topic list, mode picker and results page.
size_t polish_study_card_index(const polish_study_t *study);
