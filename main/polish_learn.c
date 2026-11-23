#include "polish_learn.h"

#include "bsp_battery.h"
#include "bsp_display.h"
#include "esp_random.h"
#include "esp_timer.h"
#include "lvgl.h"
#include "polish_audio.h"
#include "polish_course.h"
#include "polish_progress.h"
#include "polish_study.h"

LV_FONT_DECLARE(font_cn_16);
LV_FONT_DECLARE(font_polish_20);
LV_FONT_DECLARE(font_ipa_16);

enum {
    INSET = 16, WIDTH = 208, BODY_Y = 76, BODY_HEIGHT = 184,
    ROW_COUNT = 4, ROW_WIDTH = WIDTH - 20, CARD_TEXT_WIDTH = WIDTH - 16,
    PAPER = 0x101C24, PANEL = 0x1B2A35, SELECTED = 0x254D48,
    INK = 0xEFF5F1, MUTED = 0xAABCC3, ACCENT = 0x99E6CB,
    GOLD = 0xFFDB9E, RULE = 0x3C5460
};

typedef struct {
    lv_obj_t *box;
    lv_obj_t *name;
    lv_obj_t *detail;
} menu_row_t;

static polish_study_t s_study;
static lv_obj_t *s_title, *s_battery, *s_topic, *s_count;
static lv_obj_t *s_actions, *s_hint;
static menu_row_t s_rows[ROW_COUNT];
static lv_obj_t *s_face, *s_primary, *s_phonetic, *s_divider, *s_secondary;
static bool s_audio_ready;
static bool s_clip_available;
static int s_battery_soc;
static int64_t s_next_battery_read;

// The input worker samples the gauge outside LVGL. Rapid card changes reuse the
// same reading, and an unavailable gauge leaves the rest of the interface usable.
static void sample_battery(void) {
    int64_t now = esp_timer_get_time();
    if (now >= s_next_battery_read) {
        s_battery_soc = bsp_battery_soc();
        s_next_battery_read = now + 30 * 1000000LL;
    }
}

static lv_obj_t *panel(lv_obj_t *parent, int x, int y, int w, int h,
                       int radius, unsigned color) {
    lv_obj_t *obj = lv_obj_create(parent);
    lv_obj_remove_style_all(obj);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_pos(obj, x, y);
    lv_obj_set_size(obj, w, h);
    lv_obj_set_style_radius(obj, radius, 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(color), 0);
    return obj;
}

static lv_obj_t *label(lv_obj_t *parent, int x, int y, int width,
                       const lv_font_t *font, unsigned color, lv_text_align_t align) {
    lv_obj_t *obj = lv_label_create(parent);
    lv_label_set_text_static(obj, "");
    lv_label_set_long_mode(obj, LV_LABEL_LONG_MODE_WRAP);
    lv_obj_set_pos(obj, x, y);
    lv_obj_set_width(obj, width);
    lv_obj_set_style_text_font(obj, font, 0);
    lv_obj_set_style_text_color(obj, lv_color_hex(color), 0);
    lv_obj_set_style_text_align(obj, align, 0);
    lv_obj_set_style_text_line_space(obj, 1, 0);
    return obj;
}

static void visible(lv_obj_t *obj, bool show) {
    if (show) lv_obj_remove_flag(obj, LV_OBJ_FLAG_HIDDEN);
    else lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
}

static void build_screen(lv_obj_t *screen) {
    lv_obj_remove_style_all(screen);
    lv_obj_remove_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(screen, lv_color_hex(PAPER), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);
    s_title = label(screen, INSET, 14, 152, &font_cn_16, INK, LV_TEXT_ALIGN_LEFT);
    s_battery = label(screen, 169, 16, 53, &lv_font_montserrat_14, MUTED, LV_TEXT_ALIGN_RIGHT);
    panel(screen, INSET, 42, WIDTH, 1, 0, RULE);
    s_topic = label(screen, INSET, 50, 116, &font_cn_16, ACCENT, LV_TEXT_ALIGN_LEFT);
    s_count = label(screen, 132, 50, 92, &font_cn_16, MUTED, LV_TEXT_ALIGN_RIGHT);

    for (size_t i = 0; i < ROW_COUNT; ++i) {
        menu_row_t *row = &s_rows[i];
        row->box = panel(screen, INSET, BODY_Y, WIDTH, 42, 7, PANEL);
        row->name = label(row->box, 10, 0, 140, &font_cn_16, INK, LV_TEXT_ALIGN_LEFT);
        row->detail = label(row->box, 0, 0, 48, &font_cn_16, MUTED, LV_TEXT_ALIGN_RIGHT);
    }

    // A shared, vertically centred stack lets either language and the IPA wrap
    // independently without colliding at fixed baselines.
    s_face = panel(screen, INSET, BODY_Y, WIDTH, BODY_HEIGHT, 10, PANEL);
    lv_obj_set_flex_flow(s_face, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(s_face, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(s_face, 8, 0);
    s_primary = label(s_face, 0, 0, CARD_TEXT_WIDTH, &font_polish_20, INK, LV_TEXT_ALIGN_CENTER);
    s_phonetic = label(s_face, 0, 0, CARD_TEXT_WIDTH, &font_ipa_16, MUTED, LV_TEXT_ALIGN_CENTER);
    s_divider = panel(s_face, 0, 0, 48, 2, 1, RULE);
    s_secondary = label(s_face, 0, 0, CARD_TEXT_WIDTH, &font_cn_16, GOLD, LV_TEXT_ALIGN_CENTER);

    s_actions = label(screen, INSET, 267, WIDTH, &font_cn_16, INK, LV_TEXT_ALIGN_CENTER);
    s_hint = label(screen, INSET, 290, WIDTH, &font_cn_16, MUTED, LV_TEXT_ALIGN_CENTER);
}

static void footer(const char *actions, const char *hint) {
    lv_label_set_text_static(s_actions, actions);
    lv_label_set_text_static(s_hint, hint);
}

static void style_row(menu_row_t *row, int y, int height, bool selected) {
    lv_obj_set_y(row->box, y);
    lv_obj_set_height(row->box, height);
    lv_obj_set_style_bg_color(row->box, lv_color_hex(selected ? SELECTED : PANEL), 0);
    lv_obj_set_style_text_color(row->name, lv_color_hex(selected ? ACCENT : INK), 0);
    lv_obj_set_style_border_width(row->box, selected ? 1 : 0, 0);
    lv_obj_set_style_border_color(row->box, lv_color_hex(ACCENT), 0);
}

static void show_topics(void) {
    size_t total = polish_topic_count();
    lv_label_set_text_static(s_title, "波兰语学习");
    lv_label_set_text_static(s_topic, "选择主题");
    lv_label_set_text_fmt(s_count, "%u课 / %u张", (unsigned)total, (unsigned)polish_card_count());
    size_t first = s_study.topic > 0 ? s_study.topic - 1 : 0;
    if (total <= ROW_COUNT) first = 0;
    else if (first + ROW_COUNT > total) first = total - ROW_COUNT;

    for (size_t i = 0; i < ROW_COUNT; ++i) {
        size_t topic = first + i;
        menu_row_t *row = &s_rows[i];
        visible(row->box, topic < total);
        if (topic >= total) continue;
        style_row(row, BODY_Y + (int)i * 47, 42, topic == s_study.topic);
        lv_label_set_text_fmt(row->name, "%02u  %s", (unsigned)topic + 1, polish_topic_name(topic));
        if (polish_progress_completed(topic)) lv_label_set_text_static(row->detail, "已学");
        else lv_label_set_text_fmt(row->detail, "%u 张", (unsigned)polish_topic_size(topic));
        lv_obj_set_width(row->name, 140);
        lv_obj_set_width(row->detail, 48);
        lv_obj_set_style_text_align(row->detail, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_align(row->name, LV_ALIGN_LEFT_MID, 10, 0);
        lv_obj_align(row->detail, LV_ALIGN_RIGHT_MID, -10, 0);
    }
    footer("上/下 选择主题", "OK 进入课程");
}

static void show_modes(void) {
    lv_label_set_text_static(s_title, polish_topic_name(s_study.topic));
    lv_label_set_text_static(s_topic, "选择练习");
    lv_label_set_text_fmt(s_count, "共 %u 张", (unsigned)polish_topic_size(s_study.topic));
    for (size_t i = 0; i < ROW_COUNT; ++i) {
        menu_row_t *row = &s_rows[i];
        visible(row->box, i < 2);
        if (i >= 2) continue;
        bool quiz = i == 1;
        style_row(row, BODY_Y + (int)i * 94, 88, quiz == s_study.quiz_selected);
        lv_label_set_text_static(row->name, quiz ? "中文自测" : "词卡学习");
        lv_label_set_text_static(row->detail, quiz ? "看中文，回想波兰语" : "读词句，看释义和音标");
        lv_obj_set_width(row->name, ROW_WIDTH);
        lv_obj_set_width(row->detail, ROW_WIDTH);
        lv_obj_set_style_text_align(row->detail, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_align(row->name, LV_ALIGN_TOP_LEFT, 10, 16);
        lv_obj_align(row->detail, LV_ALIGN_TOP_LEFT, 10, 46);
    }
    footer("上/下 选择   OK 开始", "长按 OK 返回");
}

static void show_cards(void) {
    const polish_card_t *card = polish_card_at(polish_study_card_index(&s_study));
    bool study = s_study.page == POLISH_PAGE_STUDY;
    bool hidden = s_study.page == POLISH_PAGE_QUESTION;
    lv_label_set_text_static(s_title, study ? "词卡学习" : "中文自测");
    lv_label_set_text_static(s_topic, polish_topic_name(s_study.topic));
    lv_label_set_text_fmt(s_count, "%u / %u", (unsigned)s_study.position + 1,
                          (unsigned)s_study.round_size);
    visible(s_phonetic, !hidden);
    visible(s_divider, !hidden);
    visible(s_secondary, !hidden);
    lv_obj_set_style_text_font(s_primary, hidden ? &font_cn_16 : &font_polish_20, 0);
    lv_label_set_text_static(s_primary, hidden ? card->chinese : card->polish);
    if (hidden) {
        footer("试着用波兰语说", "OK 看答案   长按 OK 返回");
        return;
    }
    lv_label_set_text_static(s_phonetic, card->ipa);
    lv_label_set_text_static(s_secondary, card->chinese);
    if (study) {
        footer(s_clip_available ? "上/下 换卡   OK 听发音" : "上/下 换卡   暂无发音",
               "长按 OK 返回");
    } else {
        footer("上 需复习   下 已掌握",
               s_clip_available ? "OK 重听   长按 OK 返回" : "暂无发音   长按 OK 返回");
    }
}

static void show_results(void) {
    lv_label_set_text_static(s_title, "本轮结果");
    lv_label_set_text_static(s_topic, polish_topic_name(s_study.topic));
    lv_label_set_text_static(s_count, "");
    visible(s_phonetic, false);
    visible(s_divider, true);
    visible(s_secondary, true);
    lv_obj_set_style_text_font(s_primary, &lv_font_montserrat_20, 0);
    lv_label_set_text_fmt(s_primary, "%u / %u", (unsigned)(s_study.round_size - s_study.missed),
                          (unsigned)s_study.round_size);
    lv_label_set_text_static(s_secondary, "本轮已掌握");
    if (s_study.missed > 0) {
        lv_label_set_text_fmt(s_count, "待复习 %u 张", (unsigned)s_study.missed);
    }
    footer(s_study.missed ? "OK 只练待复习词卡" : "OK 再练全课", "长按 OK 返回");
}

static void render(void) {
    bool menu = s_study.page == POLISH_PAGE_TOPICS || s_study.page == POLISH_PAGE_MODES;
    visible(s_face, !menu);
    if (!menu) {
        for (size_t i = 0; i < ROW_COUNT; ++i) visible(s_rows[i].box, false);
    }
    switch (s_study.page) {
        case POLISH_PAGE_TOPICS:   show_topics(); break;
        case POLISH_PAGE_MODES:    show_modes(); break;
        case POLISH_PAGE_STUDY:
        case POLISH_PAGE_QUESTION:
        case POLISH_PAGE_ANSWER:   show_cards(); break;
        case POLISH_PAGE_RESULTS:  show_results(); break;
    }
    if (s_battery_soc >= 0) lv_label_set_text_fmt(s_battery, "%d%%", s_battery_soc);
    else lv_label_set_text_static(s_battery, "--");
}

bool polish_learn_enter(void) {
    sample_battery();
    if (!bsp_lvgl_lock(1000)) return false;
    polish_study_init(&s_study, esp_random());
    s_study.topic = polish_progress_last_topic();
    build_screen(lv_screen_active());
    render();
    bsp_lvgl_unlock();
    return true;
}

esp_err_t polish_learn_start(void) {
    esp_err_t err = polish_audio_init();
    s_audio_ready = err == ESP_OK;
    return err;
}

void polish_learn_key(bsp_btn_t btn, bsp_btn_ev_t ev) {
    polish_input_t key;
    if (btn == BSP_BTN_OK && ev == BSP_BTN_LONG) key = POLISH_INPUT_BACK;
    else if (ev != BSP_BTN_CLICK) return;
    else if (btn == BSP_BTN_OK) key = POLISH_INPUT_OK;
    else if (btn == BSP_BTN_UP) key = POLISH_INPUT_UP;
    else if (btn == BSP_BTN_DOWN) key = POLISH_INPUT_DOWN;
    else return;

    sample_battery();
    if (!bsp_lvgl_lock(500)) return;
    polish_page_t previous = s_study.page;
    polish_action_t action = polish_study_input(&s_study, key);
    if (action == POLISH_ACTION_PLAY) {
        s_clip_available = s_audio_ready &&
                           polish_audio_play((int)polish_study_card_index(&s_study)) == ESP_OK;
    } else if (action == POLISH_ACTION_REDRAW && s_audio_ready &&
               (previous == POLISH_PAGE_STUDY || previous == POLISH_PAGE_ANSWER)) {
        polish_audio_stop();
    }
    if (action != POLISH_ACTION_NONE) render();
    bsp_lvgl_unlock();
    if (previous == POLISH_PAGE_TOPICS && s_study.page == POLISH_PAGE_MODES) {
        polish_progress_opened(s_study.topic);
    } else if (previous == POLISH_PAGE_ANSWER && s_study.page == POLISH_PAGE_RESULTS &&
               s_study.missed == 0) {
        polish_progress_mastered(s_study.topic);
    }
}
