#include "polish_progress.h"
#include "polish_course.h"
#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static const char *TAG = "study_progress";
static nvs_handle_t s_store;
static bool s_writable;
static size_t s_last_topic;
static size_t s_stored_topic = SIZE_MAX;
typedef enum { UNSEEN, COMPLETED, SAVED } completion_t;
static completion_t *s_completion;

void polish_progress_init(void) {
    s_completion = calloc(polish_topic_count(), sizeof(*s_completion));
    esp_err_t err = nvs_flash_init();
    if (err == ESP_OK) err = nvs_open("polish_study", NVS_READWRITE, &s_store);
    if (err != ESP_OK) {
        // Keep study available even when NVS is full or has an incompatible version.
        ESP_LOGW(TAG, "Session-only progress: %s", esp_err_to_name(err));
        return;
    }
    s_writable = true;
    char last_key[NVS_KEY_NAME_MAX_SIZE] = {0};
    size_t len = sizeof(last_key);
    bool has_last = nvs_get_str(s_store, "_last", last_key, &len) == ESP_OK;
    for (size_t i = 0; i < polish_topic_count(); ++i) {
        const char *key = polish_topic_key(i);
        if (has_last && strcmp(last_key, key) == 0) {
            s_last_topic = s_stored_topic = i;
        }
        uint8_t value = 0;
        if (s_completion && nvs_get_u8(s_store, key, &value) == ESP_OK && value == 1) {
            s_completion[i] = SAVED;
        }
    }
}

size_t polish_progress_last_topic(void) {
    return s_last_topic;
}

bool polish_progress_completed(size_t topic) {
    return s_completion && topic < polish_topic_count() && s_completion[topic] != UNSEEN;
}

static bool commit_change(esp_err_t err) {
    if (err == ESP_OK) err = nvs_commit(s_store);
    if (err != ESP_OK) ESP_LOGW(TAG, "Saving study progress: %s", esp_err_to_name(err));
    return err == ESP_OK;
}

void polish_progress_opened(size_t topic) {
    const char *key = polish_topic_key(topic);
    if (!key) return;
    s_last_topic = topic;
    if (s_writable && s_stored_topic != topic) {
        s_stored_topic = commit_change(nvs_set_str(s_store, "_last", key)) ? topic : SIZE_MAX;
    }
}

void polish_progress_mastered(size_t topic) {
    const char *key = polish_topic_key(topic);
    if (!key || !s_completion || s_completion[topic] == SAVED) return;
    s_completion[topic] = COMPLETED;
    if (s_writable && commit_change(nvs_set_u8(s_store, key, 1))) {
        s_completion[topic] = SAVED;
    }
}
