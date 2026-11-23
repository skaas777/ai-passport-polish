#pragma once

#include <stddef.h>

// Bounds the practice queue for a single topic on the device.
enum { POLISH_TOPIC_CAPACITY = 32 };

typedef struct {
    const char *polish;
    const char *chinese;
    const char *ipa;
} polish_card_t;

// Topics and cards are zero-based. The global card index also selects its audio clip.
size_t polish_topic_count(void);
size_t polish_card_count(void);

// Invalid topics have no name, zero cards, and a first index equal to polish_card_count().
const char *polish_topic_name(size_t topic);
// Stable lowercase NVS key (at most 14 characters), or NULL for an invalid topic.
const char *polish_topic_key(size_t topic);
size_t polish_topic_first(size_t topic);
size_t polish_topic_size(size_t topic);

// Returns NULL outside the course. Valid strings live for the application's lifetime.
const polish_card_t *polish_card_at(size_t global_index);
