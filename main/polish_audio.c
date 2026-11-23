// main/polish_audio.c -- Polish pronunciation playback.
//
// ESP32-C3 is single-core: a long-lived worker task is created once in
// polish_audio_init() and driven by a length-1 queue with xQueueOverwrite().
// A new card command replaces a pending one, and a playing clip checks the
// queue so fast UP/DOWN presses interrupt cleanly instead of stacking tasks.
#include "polish_audio.h"
#include "polish_course.h"
#include <stdatomic.h>
#include <string.h>
#include "bsp_audio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

static const char *TAG = "polish_audio";

#define AUDIO_TASK_STACK   4096
#define AUDIO_TASK_PRIO    4
#define STOP_TIMEOUT_MS    2000
#define CHUNK_BYTES        512
#define CMD_STOP           (-1)
#define CMD_SHUTDOWN       (-2)

// pack_audio.py writes little-endian byte offsets followed by 11025 Hz PCM16.
extern const uint8_t polish_bank[] asm("_binary_polish_bank_bin_start");

static uint32_t clip_offset(size_t index) {
    uint32_t offset;
    memcpy(&offset, polish_bank + index * sizeof(offset), sizeof(offset));
    return offset;
}

static atomic_bool       s_playing;
static TaskHandle_t      s_task;
static QueueHandle_t     s_cmd_queue;   // length 1, int32_t: >=0 card index, -1 stop
static SemaphoreHandle_t s_stopped;

static bool play_clip(int card_index) {
    uint32_t start = clip_offset(card_index);
    size_t length = clip_offset((size_t)card_index + 1) - start;

    // Same format on every call is cheap (bsp_audio_set_format reuses the
    // already-open codec); keep it here so wake-from-sleep restores it too.
    if (bsp_audio_set_format(11025, 16, 1) != ESP_OK) {
        ESP_LOGW(TAG, "audio format failed");
        return false;
    }
    bsp_audio_set_volume(80);

    size_t offset = 0;
    while (offset < length) {
        // A newer command (or STOP) must be able to interrupt the current word.
        int32_t pending;
        if (xQueuePeek(s_cmd_queue, &pending, 0) == pdTRUE) break;

        size_t remaining = length - offset;
        size_t n = remaining < CHUNK_BYTES ? remaining : CHUNK_BYTES;
        if (bsp_audio_write(polish_bank + start + offset, n) != ESP_OK) break;
        offset += n;
    }
    return offset >= length;
}

static void audio_task(void *arg) {
    (void)arg;
    for (;;) {
        int32_t cmd;
        if (xQueueReceive(s_cmd_queue, &cmd, portMAX_DELAY) != pdTRUE) continue;
        if (cmd == CMD_SHUTDOWN) break;
        if (cmd == CMD_STOP) continue;
        atomic_store(&s_playing, true);
        play_clip((int)cmd);
        atomic_store(&s_playing, false);
    }
    // The lifecycle owner keeps the handle until this acknowledgement arrives.
    xSemaphoreGive(s_stopped);
    for (;;) vTaskSuspend(NULL);
}

esp_err_t polish_audio_init(void) {
    if (s_task) return ESP_OK;
    if (clip_offset(0) != (polish_card_count() + 1) * sizeof(uint32_t)) {
        ESP_LOGE(TAG, "Audio index and course size differ");
        return ESP_ERR_INVALID_SIZE;
    }

    if (s_stopped) {                            // stale handle from a timed-out stop
        vSemaphoreDelete(s_stopped);
        s_stopped = NULL;
    }
    s_stopped = xSemaphoreCreateBinary();
    if (!s_stopped) return ESP_ERR_NO_MEM;

    s_cmd_queue = xQueueCreate(1, sizeof(int32_t));
    if (!s_cmd_queue) {
        vSemaphoreDelete(s_stopped);
        s_stopped = NULL;
        return ESP_ERR_NO_MEM;
    }

    esp_err_t ret = bsp_audio_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "BSP audio init failed: %s", esp_err_to_name(ret));
        vQueueDelete(s_cmd_queue);
        s_cmd_queue = NULL;
        vSemaphoreDelete(s_stopped);
        s_stopped = NULL;
        return ret;
    }

    if (xTaskCreate(audio_task, "polish_audio", AUDIO_TASK_STACK, NULL,
                    AUDIO_TASK_PRIO, &s_task) != pdPASS) {
        vQueueDelete(s_cmd_queue);
        s_cmd_queue = NULL;
        vSemaphoreDelete(s_stopped);
        s_stopped = NULL;
        return ESP_ERR_NO_MEM;
    }

    ESP_LOGI(TAG, "audio ready, %d clips", (int)polish_card_count());
    return ESP_OK;
}

esp_err_t polish_audio_play(int card_index) {
    if (!s_task || !s_cmd_queue) return ESP_ERR_INVALID_STATE;
    if (card_index < 0 || card_index >= (int)polish_card_count()) return ESP_ERR_INVALID_ARG;
    int32_t cmd = (int32_t)card_index;
    xQueueOverwrite(s_cmd_queue, &cmd);        // never blocks, safe from key callback
    return ESP_OK;
}

esp_err_t polish_audio_stop(void) {
    if (!s_task || !s_cmd_queue) return ESP_OK;   // safe when never initialized
    int32_t cmd = CMD_STOP;
    xQueueOverwrite(s_cmd_queue, &cmd);
    return ESP_OK;
}

bool polish_audio_is_playing(void) {
    return atomic_load(&s_playing);
}

esp_err_t polish_audio_deinit(void) {
    TaskHandle_t task = s_task;
    if (!task) {
        if (s_cmd_queue) { vQueueDelete(s_cmd_queue); s_cmd_queue = NULL; }
        if (s_stopped) { vSemaphoreDelete(s_stopped); s_stopped = NULL; }
        return ESP_OK;
    }

    int32_t cmd = CMD_SHUTDOWN;
    xQueueOverwrite(s_cmd_queue, &cmd);
    if (xSemaphoreTake(s_stopped, pdMS_TO_TICKS(STOP_TIMEOUT_MS)) != pdTRUE) {
        // Do not delete a task that may still be inside bsp_audio_write().
        ESP_LOGW(TAG, "audio stop timed out; retry later");
        return ESP_ERR_TIMEOUT;
    }

    vTaskDelete(task);
    s_task = NULL;
    vQueueDelete(s_cmd_queue);
    s_cmd_queue = NULL;
    vSemaphoreDelete(s_stopped);
    s_stopped = NULL;
    return ESP_OK;
}
