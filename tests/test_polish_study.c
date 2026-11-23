#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "polish_study.h"

static void check_courses(void) {
    size_t count = 0;
    for (size_t topic = 0; topic < polish_topic_count(); ++topic) {
        assert(polish_topic_name(topic)[0]);
        const char *key = polish_topic_key(topic);
        assert(key && key[0] && strlen(key) <= 14);
        assert(strspn(key, "abcdefghijklmnopqrstuvwxyz") == strlen(key));
        for (size_t i = 0; i < topic; ++i) assert(strcmp(key, polish_topic_key(i)) != 0);
        assert(polish_topic_first(topic) == count);
        size_t size = polish_topic_size(topic);
        assert(size > 0 && size <= POLISH_TOPIC_CAPACITY);
        for (size_t i = 0; i < size; ++i) {
            const polish_card_t *card = polish_card_at(count + i);
            assert(card && card->polish[0] && card->chinese[0] && card->ipa[0]);
        }
        count += size;
    }
    assert(count > 0 && count == polish_card_count());
    assert(!polish_card_at(count));
}

static void check_study_navigation(void) {
    polish_study_t s;
    polish_study_init(&s, 0);
    assert(s.page == POLISH_PAGE_TOPICS && s.topic == 0);
    assert(polish_study_input(&s, POLISH_INPUT_BACK) == POLISH_ACTION_NONE);
    assert(polish_study_card_index(&s) == SIZE_MAX);
    assert(polish_study_input(&s, POLISH_INPUT_UP) == POLISH_ACTION_REDRAW);
    assert(s.topic == polish_topic_count() - 1);

    // Walk every lesson, including both wrap boundaries, through the real course API.
    for (size_t topic = 0; topic < polish_topic_count(); ++topic) {
        polish_study_input(&s, POLISH_INPUT_DOWN);
        assert(s.topic == topic);
        polish_study_input(&s, POLISH_INPUT_OK);
        assert(s.page == POLISH_PAGE_MODES && !s.quiz_selected);
        assert(polish_study_input(&s, POLISH_INPUT_OK) == POLISH_ACTION_PLAY);
        assert(s.page == POLISH_PAGE_STUDY && s.position == 0);
        assert(s.round_size == polish_topic_size(topic));
        size_t first = polish_topic_first(topic);
        assert(polish_study_card_index(&s) == first);
        assert(polish_study_input(&s, POLISH_INPUT_OK) == POLISH_ACTION_PLAY);
        assert(polish_study_input(&s, POLISH_INPUT_UP) == POLISH_ACTION_PLAY);
        assert(polish_study_card_index(&s) == first + s.round_size - 1);
        for (size_t i = 0; i < s.round_size; ++i) {
            assert(polish_study_input(&s, POLISH_INPUT_DOWN) == POLISH_ACTION_PLAY);
            assert(polish_study_card_index(&s) == first + i);
        }
        polish_study_input(&s, POLISH_INPUT_BACK);
        assert(s.page == POLISH_PAGE_MODES && polish_study_card_index(&s) == SIZE_MAX);
        polish_study_input(&s, POLISH_INPUT_BACK);
        assert(s.page == POLISH_PAGE_TOPICS && s.topic == topic);
    }
}

static void check_quiz_round(polish_study_t *s, const bool *expected, const bool *mark_again) {
    size_t size = polish_topic_size(s->topic);
    size_t expected_count = 0;
    for (size_t i = 0; i < size; ++i) expected_count += expected[i];
    assert(s->round_size == expected_count);
    bool seen[POLISH_TOPIC_CAPACITY] = {false};
    size_t missed = 0;
    for (size_t i = 0; i < expected_count; ++i) {
        assert(s->page == POLISH_PAGE_QUESTION && s->position == i);
        size_t index = polish_study_card_index(s);
        size_t offset = index - polish_topic_first(s->topic);
        assert(offset < size && expected[offset] && !seen[offset]);
        seen[offset] = true;
        // Direction keys cannot skip or grade a hidden answer.
        assert(polish_study_input(s, POLISH_INPUT_UP) == POLISH_ACTION_NONE);
        assert(polish_study_input(s, POLISH_INPUT_DOWN) == POLISH_ACTION_NONE);
        assert(polish_study_input(s, POLISH_INPUT_OK) == POLISH_ACTION_PLAY);
        assert(s->page == POLISH_PAGE_ANSWER && polish_study_card_index(s) == index);
        assert(polish_study_input(s, POLISH_INPUT_OK) == POLISH_ACTION_PLAY);
        assert(s->position == i && s->missed == missed);
        polish_input_t grade = mark_again[offset] ? POLISH_INPUT_UP : POLISH_INPUT_DOWN;
        assert(polish_study_input(s, grade) == POLISH_ACTION_REDRAW);
        missed += mark_again[offset];
        assert(s->missed == missed);
    }
    assert(s->page == POLISH_PAGE_RESULTS && s->position == expected_count);
    assert(polish_study_card_index(s) == SIZE_MAX);
}

static void check_retry_rounds(size_t topic) {
    polish_study_t s;
    polish_study_init(&s, (uint32_t)topic);
    for (size_t i = 0; i < topic; ++i) polish_study_input(&s, POLISH_INPUT_DOWN);
    polish_study_input(&s, POLISH_INPUT_OK);
    polish_study_input(&s, POLISH_INPUT_DOWN);
    assert(s.quiz_selected);
    assert(polish_study_input(&s, POLISH_INPUT_OK) == POLISH_ACTION_REDRAW);

    bool all[POLISH_TOPIC_CAPACITY] = {false};
    bool alternate[POLISH_TOPIC_CAPACITY] = {false};
    bool one[POLISH_TOPIC_CAPACITY] = {true};
    bool none[POLISH_TOPIC_CAPACITY] = {false};
    for (size_t i = 0; i < polish_topic_size(topic); ++i) {
        all[i] = true;
        alternate[i] = i % 2 == 0;
    }
    check_quiz_round(&s, all, all);
    polish_study_input(&s, POLISH_INPUT_OK);
    check_quiz_round(&s, all, alternate);
    polish_study_input(&s, POLISH_INPUT_OK);
    check_quiz_round(&s, alternate, one);
    polish_study_input(&s, POLISH_INPUT_OK);
    check_quiz_round(&s, one, none);
    // Restart a mastered topic; abandoning a partially graded round discards its misses.
    polish_study_input(&s, POLISH_INPUT_OK);
    assert(s.round_size == polish_topic_size(topic));
    polish_study_input(&s, POLISH_INPUT_OK);
    polish_study_input(&s, POLISH_INPUT_UP);
    polish_study_input(&s, POLISH_INPUT_BACK);
    assert(s.page == POLISH_PAGE_MODES && s.quiz_selected);
    polish_study_input(&s, POLISH_INPUT_OK);
    check_quiz_round(&s, all, none);
    polish_study_input(&s, POLISH_INPUT_BACK);
    assert(s.page == POLISH_PAGE_MODES && s.topic == topic);
}

int main(void) {
    check_courses();
    check_study_navigation();
    for (size_t topic = 0; topic < polish_topic_count(); ++topic) check_retry_rounds(topic);
    return 0;
}
