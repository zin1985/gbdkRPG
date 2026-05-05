#include <gb/gb.h>
#include "dialogue.h"
#include "jpfont.h"

static UINT8 dialogue_active = 0u;

static void dialogue_wait_a_newpress(void) {
    waitpadup();
    waitpad(J_A);
    waitpadup();
}

static UINT8 dialogue_render_page(const char *text, const char **next_ptr) {
    UINT8 col = 1u;
    UINT8 row = 1u;
    UINT8 consumed;
    UINT8 done = 0u;
    const char *p = text;

    jp_window_prepare();
    jp_window_clear_text();

    while (*p) {
        if (*p == '\f') { p++; break; }
        if (*p == '\n') {
            p++;
            col = 1u;
            row++;
            if (row > JP_TEXT_ROWS) break;
            continue;
        }
        if (col > JP_TEXT_COLS) {
            col = 1u;
            row++;
            if (row > JP_TEXT_ROWS) break;
        }

        jp_put_glyph_utf8(col, row, p, &consumed);
        p += (consumed == 0u) ? 1 : consumed;
        col++;
        jp_wait_vbl(1u);
    }

    if (*p == '\0') done = 1u;
    *next_ptr = p;
    return done;
}

void dialogue_init(void) {
    jp_init();
    dialogue_active = 0u;
}

void dialogue_hide(void) {
    dialogue_active = 0u;
    jp_window_hide();
}

UINT8 dialogue_is_active(void) {
    return dialogue_active;
}

void dialogue_message_nowait_window(const char *text) {
    const char *next_ptr;
    dialogue_active = 1u;
    dialogue_render_page(text, &next_ptr);
}

void dialogue_message(const char *text) {
    const char *p = text;
    UINT8 done = 0u;
    dialogue_active = 1u;

    while (!done) {
        done = dialogue_render_page(p, &p);
        dialogue_wait_a_newpress();
    }

    dialogue_hide();
}
