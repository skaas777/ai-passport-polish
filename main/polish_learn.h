#pragma once

#include <stdbool.h>
#include "bsp_button.h"
#include "esp_err.h"

// Enter once after display initialization. The application owns the active screen.
bool polish_learn_enter(void);
// Call before enabling input. Text learning is available even if audio init fails.
esp_err_t polish_learn_start(void);
// Call from the input worker, outside the LVGL lock.
void polish_learn_key(bsp_btn_t btn, bsp_btn_ev_t ev);
