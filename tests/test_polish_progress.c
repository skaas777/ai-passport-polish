// Run each scenario in a new process, just as a device boots with fresh RAM.
#include "polish_progress.h"
#include "polish_course.h"
#include "nvs.h"
#include "nvs_flash.h"
#include <assert.h>
#include <stdbool.h>
#include <string.h>

static const char *last_key = "transport";
static const char *mastered_key = "food";
static bool offline, fail_commit;
static unsigned writes, commits;

esp_err_t nvs_flash_init(void) { return offline ? ESP_FAIL : ESP_OK; }

esp_err_t nvs_open(const char *name, int mode, nvs_handle_t *out) {
    assert(!offline && strcmp(name, "polish_study") == 0 && mode == NVS_READWRITE);
    *out = 1;
    return ESP_OK;
}

esp_err_t nvs_get_str(nvs_handle_t h, const char *key, char *value, size_t *size) {
    assert(h == 1 && strcmp(key, "_last") == 0 && *size > strlen(last_key));
    strcpy(value, last_key);
    return ESP_OK;
}

esp_err_t nvs_set_str(nvs_handle_t h, const char *key, const char *value) {
    assert(h == 1 && strcmp(key, "_last") == 0);
    last_key = value;
    ++writes;
    return ESP_OK;
}

esp_err_t nvs_get_u8(nvs_handle_t h, const char *key, uint8_t *value) {
    assert(h == 1);
    if (strcmp(key, mastered_key) != 0) return ESP_FAIL;
    *value = 1;
    return ESP_OK;
}

esp_err_t nvs_set_u8(nvs_handle_t h, const char *key, uint8_t value) {
    assert(h == 1 && value == 1);
    mastered_key = key;
    ++writes;
    return ESP_OK;
}

esp_err_t nvs_commit(nvs_handle_t h) {
    assert(h == 1);
    ++commits;
    return fail_commit ? ESP_FAIL : ESP_OK;
}

int main(int argc, char **argv) {
    assert(argc == 2);
    offline = strcmp(argv[1], "offline") == 0;
    bool unknown = strcmp(argv[1], "unknown") == 0;
    if (unknown) last_key = "retired";
    polish_progress_init();
    size_t last = polish_progress_last_topic();
    if (offline || unknown) assert(last == 0);
    else assert(strcmp(polish_topic_key(last), "transport") == 0);
    for (size_t i = 0; i < polish_topic_count(); ++i) {
        bool expected = !offline && strcmp(polish_topic_key(i), "food") == 0;
        assert(polish_progress_completed(i) == expected);
    }
    polish_progress_opened(SIZE_MAX);
    polish_progress_mastered(SIZE_MAX);
    assert(!polish_progress_completed(SIZE_MAX));
    assert(writes == 0);

    size_t next = 1;
    fail_commit = true;
    polish_progress_opened(next);
    polish_progress_mastered(next);
    assert(polish_progress_last_topic() == next && polish_progress_completed(next));
    assert(writes == (offline ? 0u : 2u));

    fail_commit = false;
    polish_progress_opened(next);
    polish_progress_mastered(next);
    polish_progress_opened(next);
    polish_progress_mastered(next);
    assert(writes == (offline ? 0u : 4u));
    assert(commits == writes);
    return 0;
}
