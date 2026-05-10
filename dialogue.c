#pragma bank 5

#include <gb/gb.h>
#include "dialogue.h"
#include "jpfont.h"
#include "audio.h"

BANKREF(dialogue_bank)

static UINT8 dialogue_active;

#define DIALOGUE_FIELD_OAM_COUNT 16u
#define DIALOGUE_HIDE_Y ((UINT8)(JP_WIN_Y + 8u))

static UINT8 dialogue_saved_x[DIALOGUE_FIELD_OAM_COUNT];
static UINT8 dialogue_saved_y[DIALOGUE_FIELD_OAM_COUNT];
static UINT8 dialogue_saved_mask[DIALOGUE_FIELD_OAM_COUNT];

static void dialogue_restore_overlapped_sprites(void) BANKED;

static void dialogue_hide_overlapped_sprites(void) BANKED {
    UINT8 i;
    for (i = 0u; i < DIALOGUE_FIELD_OAM_COUNT; i++) {
        dialogue_saved_mask[i] = 0u;
        dialogue_saved_x[i] = shadow_OAM[i].x;
        dialogue_saved_y[i] = shadow_OAM[i].y;
        if (shadow_OAM[i].x != 0u && shadow_OAM[i].y > DIALOGUE_HIDE_Y) {
            dialogue_saved_mask[i] = 1u;
            move_sprite(i, 0u, 0u);
        }
    }
    SHOW_SPRITES;
}

static void dialogue_restore_overlapped_sprites(void) BANKED {
    UINT8 i;
    for (i = 0u; i < DIALOGUE_FIELD_OAM_COUNT; i++) {
        if (dialogue_saved_mask[i]) {
            move_sprite(i, dialogue_saved_x[i], dialogue_saved_y[i]);
            dialogue_saved_mask[i] = 0u;
        }
    }
    SHOW_SPRITES;
}


static void dialogue_wait_a_newpress(void) {
    audio_waitpadup();
    audio_waitpad(J_A);
    audio_waitpadup();
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
        audio_wait_vbl();
    }

    if (*p == '\0') done = 1u;
    *next_ptr = p;
    return done;
}

void dialogue_init(void) BANKED {
    jp_init();
}

void dialogue_hide(void) BANKED {
    dialogue_active = 0u;
    jp_window_hide();
    dialogue_restore_overlapped_sprites();
}

UINT8 dialogue_is_active(void) BANKED {
    return dialogue_active;
}

void dialogue_message_nowait_window(const char *text) BANKED {
    const char *next_ptr;
    dialogue_hide_overlapped_sprites();
    dialogue_active = 1u;
    dialogue_render_page(text, &next_ptr);
}

void dialogue_message(const char *text) BANKED {
    const char *p = text;
    UINT8 done = 0u;
    dialogue_hide_overlapped_sprites();
    dialogue_active = 1u;

    while (!done) {
        done = dialogue_render_page(p, &p);
        dialogue_wait_a_newpress();
    }

    dialogue_hide();
}
