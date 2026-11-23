#include "polish_study.h"

_Static_assert(POLISH_TOPIC_CAPACITY > 0 && POLISH_TOPIC_CAPACITY <= UINT8_MAX + 1,
               "A topic-local card offset must fit in one byte");

void polish_study_init(polish_study_t *study, uint32_t seed) {
    *study = (polish_study_t){ .page = POLISH_PAGE_TOPICS, .shuffle_state = seed };
}

static size_t step(size_t position, size_t count, polish_input_t key) {
    if (key == POLISH_INPUT_UP) return position == 0 ? count - 1 : position - 1;
    return (position + 1) % count;
}

static void start_quiz(polish_study_t *study) {
    // While grading, we collect missed offsets at the front of the same deck.
    // That prefix is the complete input for the next round.
    if (study->missed) {
        study->round_size = study->missed;
    } else {
        study->round_size = polish_topic_size(study->topic);
        for (size_t i = 0; i < study->round_size; ++i) study->deck[i] = (uint8_t)i;
    }
    for (size_t n = study->round_size; n > 1; --n) {
        // Fisher-Yates with a small local PRNG; independent of hardware and libc rand().
        study->shuffle_state = study->shuffle_state * UINT32_C(1664525) + UINT32_C(1013904223);
        size_t i = ((uint64_t)study->shuffle_state * n) >> 32;
        uint8_t tmp = study->deck[n - 1];
        study->deck[n - 1] = study->deck[i];
        study->deck[i] = tmp;
    }
    study->position = 0;
    study->missed = 0;
    study->page = POLISH_PAGE_QUESTION;
}

polish_action_t polish_study_input(polish_study_t *study, polish_input_t key) {
    if (key == POLISH_INPUT_BACK) {
        if (study->page == POLISH_PAGE_TOPICS) return POLISH_ACTION_NONE;
        study->page = study->page == POLISH_PAGE_MODES ? POLISH_PAGE_TOPICS : POLISH_PAGE_MODES;
        return POLISH_ACTION_REDRAW;
    }

    bool move = key == POLISH_INPUT_UP || key == POLISH_INPUT_DOWN;
    switch (study->page) {
        case POLISH_PAGE_TOPICS:
            if (!polish_topic_count()) break;
            if (move) {
                study->topic = step(study->topic, polish_topic_count(), key);
                return POLISH_ACTION_REDRAW;
            }
            if (key == POLISH_INPUT_OK) {
                study->quiz_selected = false;
                study->page = POLISH_PAGE_MODES;
                return POLISH_ACTION_REDRAW;
            }
            break;

        case POLISH_PAGE_MODES:
            if (move) {
                study->quiz_selected = !study->quiz_selected;
                return POLISH_ACTION_REDRAW;
            }
            if (key != POLISH_INPUT_OK) break;
            study->round_size = polish_topic_size(study->topic);
            if (!study->round_size) break;
            study->position = 0;
            study->missed = 0;
            if (study->quiz_selected) {
                if (study->round_size > POLISH_TOPIC_CAPACITY) break;
                start_quiz(study);
                return POLISH_ACTION_REDRAW;
            }
            study->page = POLISH_PAGE_STUDY;
            return POLISH_ACTION_PLAY;

        case POLISH_PAGE_STUDY:
            if (move) study->position = step(study->position, study->round_size, key);
            if (move || key == POLISH_INPUT_OK) return POLISH_ACTION_PLAY;
            break;

        case POLISH_PAGE_QUESTION:
            if (key == POLISH_INPUT_OK) {
                study->page = POLISH_PAGE_ANSWER;
                return POLISH_ACTION_PLAY;
            }
            break;

        case POLISH_PAGE_ANSWER:
            if (key == POLISH_INPUT_OK) return POLISH_ACTION_PLAY;
            if (move) {
                if (key == POLISH_INPUT_UP) {
                    study->deck[study->missed++] = study->deck[study->position];
                }
                ++study->position;
                study->page = study->position == study->round_size ? POLISH_PAGE_RESULTS
                                                                   : POLISH_PAGE_QUESTION;
                return POLISH_ACTION_REDRAW;
            }
            break;

        case POLISH_PAGE_RESULTS:
            if (key == POLISH_INPUT_OK) {
                start_quiz(study);
                return POLISH_ACTION_REDRAW;
            }
            break;
    }
    return POLISH_ACTION_NONE;
}

size_t polish_study_card_index(const polish_study_t *study) {
    size_t first = polish_topic_first(study->topic);
    switch (study->page) {
        case POLISH_PAGE_STUDY:
            return first + study->position;
        case POLISH_PAGE_QUESTION:
        case POLISH_PAGE_ANSWER:
            return first + study->deck[study->position];
        default:
            return SIZE_MAX;
    }
}
