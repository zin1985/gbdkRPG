#pragma bank 5

#include <gb/gb.h>
#include "save_runtime.h"
#include "jpfont.h"
#include "audio.h"

BANKREF(save_runtime_bank)

#define SAVE_MAGIC0 0x52u /* R */
#define SAVE_MAGIC1 0x50u /* P */
#define SAVE_VERSION 2u
#define SAVE_SLOT_BYTES 512u
#define SAVE_HEADER_SIZE 5u

static UINT8 save_cursor;

static UINT8 *save_sram_ptr(UINT8 slot, UINT16 offset) BANKED {
    return (UINT8 *)(0xA000u + ((UINT16)slot * SAVE_SLOT_BYTES) + offset);
}

static UINT16 save_checksum_snapshot(const SaveSnapshot *snapshot) BANKED {
    const UINT8 *p;
    UINT16 sum;
    UINT16 i;

    if (snapshot == 0) return 0u;
    p = (const UINT8 *)snapshot;
    sum = 0x1357u;
    for (i = 0u; i < (UINT16)sizeof(SaveSnapshot); i++) {
        sum = (UINT16)(sum + p[i]);
        sum = (UINT16)((sum << 1) | (sum >> 15));
    }
    return sum;
}

static void save_clear_screen(void) BANKED {
    DISPLAY_OFF;
    HIDE_WIN;
    SHOW_BKG;
    HIDE_SPRITES;
    move_bkg(0u, 0u);
    jp_bkg_clear_area(0u, 0u, 20u, 18u);
    jp_draw_bkg_frame(0u, 0u, 20u, 18u);
    DISPLAY_ON;
}

static void save_u16_to_dec(UINT16 value, char *out) BANKED {
    UINT8 i;
    UINT8 started;
    UINT16 div;

    i = 0u;
    started = 0u;
    div = 10000u;
    while (div > 0u) {
        UINT8 digit = 0u;
        while (value >= div) {
            value = (UINT16)(value - div);
            digit++;
        }
        if (digit != 0u || started || div == 1u) {
            out[i++] = (char)('0' + digit);
            started = 1u;
        }
        div /= 10u;
    }
    out[i] = '\0';
}

static void save_draw_cursor(UINT8 base_y, UINT8 count) BANKED {
    UINT8 i;
    for (i = 0u; i < count; i++) {
        jp_put_bkg_text(2u, (UINT8)(base_y + i), (save_cursor == i) ? ">" : " ");
    }
}

UINT8 save_runtime_slot_has_data(UINT8 slot) BANKED {
    UINT8 ok;
    UINT8 m0;
    UINT8 m1;
    UINT8 ver;

    if (slot >= SAVE_SLOT_COUNT) return 0u;

    ENABLE_RAM;
    SWITCH_RAM(0u);
    m0 = *save_sram_ptr(slot, 0u);
    m1 = *save_sram_ptr(slot, 1u);
    ver = *save_sram_ptr(slot, 2u);
    DISABLE_RAM;

    ok = (UINT8)(m0 == SAVE_MAGIC0 && m1 == SAVE_MAGIC1 && ver == SAVE_VERSION);
    return ok;
}

UINT8 save_runtime_save_slot(UINT8 slot, const SaveSnapshot *snapshot) BANKED {
    UINT16 checksum;
    UINT16 i;
    const UINT8 *src;
    UINT8 *dst;

    if (slot >= SAVE_SLOT_COUNT || snapshot == 0) return 0u;

    checksum = save_checksum_snapshot(snapshot);
    src = (const UINT8 *)snapshot;

    ENABLE_RAM;
    SWITCH_RAM(0u);
    *save_sram_ptr(slot, 0u) = SAVE_MAGIC0;
    *save_sram_ptr(slot, 1u) = SAVE_MAGIC1;
    *save_sram_ptr(slot, 2u) = SAVE_VERSION;
    *save_sram_ptr(slot, 3u) = (UINT8)(checksum & 0xFFu);
    *save_sram_ptr(slot, 4u) = (UINT8)(checksum >> 8u);
    dst = save_sram_ptr(slot, SAVE_HEADER_SIZE);
    for (i = 0u; i < (UINT16)sizeof(SaveSnapshot); i++) {
        dst[i] = src[i];
    }
    DISABLE_RAM;

    return 1u;
}

UINT8 save_runtime_load_slot(UINT8 slot, SaveSnapshot *out) BANKED {
    UINT16 checksum;
    UINT16 stored;
    UINT16 i;
    UINT8 *dst;
    UINT8 *src;
    UINT8 m0;
    UINT8 m1;
    UINT8 ver;

    if (slot >= SAVE_SLOT_COUNT || out == 0) return 0u;

    ENABLE_RAM;
    SWITCH_RAM(0u);
    m0 = *save_sram_ptr(slot, 0u);
    m1 = *save_sram_ptr(slot, 1u);
    ver = *save_sram_ptr(slot, 2u);
    stored = (UINT16)*save_sram_ptr(slot, 3u) | ((UINT16)*save_sram_ptr(slot, 4u) << 8u);
    if (m0 != SAVE_MAGIC0 || m1 != SAVE_MAGIC1 || ver != SAVE_VERSION) {
        DISABLE_RAM;
        return 0u;
    }
    dst = (UINT8 *)out;
    src = save_sram_ptr(slot, SAVE_HEADER_SIZE);
    for (i = 0u; i < (UINT16)sizeof(SaveSnapshot); i++) {
        dst[i] = src[i];
    }
    DISABLE_RAM;

    checksum = save_checksum_snapshot(out);
    return (checksum == stored) ? 1u : 0u;
}

static void save_draw_slot_list(const char *title) BANKED {
    UINT8 i;
    char nbuf[6];

    save_clear_screen();
    jp_put_bkg_text(1u, 1u, title);
    for (i = 0u; i < SAVE_SLOT_COUNT; i++) {
        jp_put_bkg_text(3u, (UINT8)(4u + i), "スロット");
        save_u16_to_dec((UINT16)(i + 1u), nbuf);
        jp_put_bkg_text(11u, (UINT8)(4u + i), nbuf);
        jp_put_bkg_text(13u, (UINT8)(4u + i), save_runtime_slot_has_data(i) ? "あり" : "なし");
    }
    jp_put_bkg_text(1u, 16u, "A けってい  B もどる");
    save_draw_cursor(4u, SAVE_SLOT_COUNT);
}

UINT8 save_runtime_save_select(const SaveSnapshot *snapshot) BANKED {
    UINT8 keys;

    save_cursor = 0u;
    save_draw_slot_list("セーブを えらぶ");
    while (1) {
        keys = audio_waitpad(J_UP | J_DOWN | J_A | J_B | J_START);
        audio_waitpadup();
        if (keys & J_UP) {
            if (save_cursor == 0u) save_cursor = SAVE_SLOT_COUNT - 1u;
            else save_cursor--;
            save_draw_cursor(4u, SAVE_SLOT_COUNT);
        } else if (keys & J_DOWN) {
            save_cursor++;
            if (save_cursor >= SAVE_SLOT_COUNT) save_cursor = 0u;
            save_draw_cursor(4u, SAVE_SLOT_COUNT);
        } else if (keys & J_A) {
            if (save_runtime_save_slot(save_cursor, snapshot)) {
                jp_bkg_clear_area(1u, 12u, 18u, 2u);
                jp_put_bkg_text(2u, 12u, "セーブしました");
            } else {
                jp_bkg_clear_area(1u, 12u, 18u, 2u);
                jp_put_bkg_text(2u, 12u, "セーブしっぱい");
            }
            audio_waitpad(J_A | J_B | J_START);
            audio_waitpadup();
            return 1u;
        } else if (keys & (J_B | J_START)) {
            return 0u;
        }
    }
}

static UINT8 save_runtime_load_select(SaveSnapshot *out) BANKED {
    UINT8 keys;

    save_cursor = 0u;
    save_draw_slot_list("つづきから");
    while (1) {
        keys = audio_waitpad(J_UP | J_DOWN | J_A | J_B | J_START);
        audio_waitpadup();
        if (keys & J_UP) {
            if (save_cursor == 0u) save_cursor = SAVE_SLOT_COUNT - 1u;
            else save_cursor--;
            save_draw_cursor(4u, SAVE_SLOT_COUNT);
        } else if (keys & J_DOWN) {
            save_cursor++;
            if (save_cursor >= SAVE_SLOT_COUNT) save_cursor = 0u;
            save_draw_cursor(4u, SAVE_SLOT_COUNT);
        } else if (keys & J_A) {
            if (save_runtime_load_slot(save_cursor, out)) {
                return 1u;
            }
            jp_bkg_clear_area(1u, 12u, 18u, 2u);
            jp_put_bkg_text(2u, 12u, "データが ありません");
        } else if (keys & (J_B | J_START)) {
            return 0u;
        }
    }
}

UINT8 save_runtime_title_load(SaveSnapshot *out) BANKED {
    UINT8 keys;
    UINT8 has_save;

    save_cursor = 0u;
    has_save = (UINT8)(save_runtime_slot_has_data(0u) || save_runtime_slot_has_data(1u) || save_runtime_slot_has_data(2u));

    save_clear_screen();
    jp_put_bkg_text(5u, 3u, "GBDK RPG");
    jp_put_bkg_text(4u, 8u, "はじめから");
    jp_put_bkg_text(4u, 10u, "つづきから");
    jp_put_bkg_text(1u, 16u, "A けってい");
    save_draw_cursor(8u, 2u);

    while (1) {
        keys = audio_waitpad(J_UP | J_DOWN | J_A | J_START);
        audio_waitpadup();
        if (keys & (J_UP | J_DOWN)) {
            save_cursor ^= 1u;
            save_draw_cursor(8u, 2u);
        } else if (keys & (J_A | J_START)) {
            if (save_cursor == 0u) return SAVE_TITLE_NEW;
            if (!has_save) {
                jp_bkg_clear_area(1u, 13u, 18u, 2u);
                jp_put_bkg_text(2u, 13u, "セーブが ありません");
            } else if (save_runtime_load_select(out)) {
                return SAVE_TITLE_CONTINUE;
            } else {
                save_clear_screen();
                jp_put_bkg_text(5u, 3u, "GBDK RPG");
                jp_put_bkg_text(4u, 8u, "はじめから");
                jp_put_bkg_text(4u, 10u, "つづきから");
                jp_put_bkg_text(1u, 16u, "A けってい");
                save_draw_cursor(8u, 2u);
            }
        }
    }
}
