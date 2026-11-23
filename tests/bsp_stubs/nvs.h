#pragma once
#include "esp_err.h"
#include <stddef.h>
#include <stdint.h>

#define NVS_KEY_NAME_MAX_SIZE 16
#define NVS_READWRITE 1
typedef uint32_t nvs_handle_t;

esp_err_t nvs_open(const char *name, int mode, nvs_handle_t *out);
esp_err_t nvs_get_str(nvs_handle_t handle, const char *key, char *value, size_t *size);
esp_err_t nvs_set_str(nvs_handle_t handle, const char *key, const char *value);
esp_err_t nvs_get_u8(nvs_handle_t handle, const char *key, uint8_t *value);
esp_err_t nvs_set_u8(nvs_handle_t handle, const char *key, uint8_t value);
esp_err_t nvs_commit(nvs_handle_t handle);
