#pragma bank 5

#include <gb/gb.h>
#include <gb/hardware.h>
#include <gb/cgb.h>
#include "save_runtime.h"
#include "jpfont.h"
#include "audio.h"

BANKREF(save_runtime_bank)

#define SAVE_MAGIC0 0x52u /* R */
#define SAVE_MAGIC1 0x50u /* P */
#define SAVE_VERSION 4u
#define SAVE_SLOT_BYTES 512u
#define SAVE_HEADER_SIZE 5u

static UINT8 save_cursor;

#ifndef VBK_TILES
#define VBK_TILES 0u
#endif
#ifndef VBK_ATTRIBUTES
#define VBK_ATTRIBUTES 1u
#endif
#define CGB_PAL_AUTOINC 0x80u
#define CGB_RGB5(r, g, b) ((UINT16)((r) | ((UINT16)(g) << 5) | ((UINT16)(b) << 10)))

static UINT8 save_cgb_palette_ready;
static const UINT16 save_cgb_ui_bg_pal0[4u] = {
    CGB_RGB5(31,31,28), CGB_RGB5(23,22,18), CGB_RGB5(13,13,12), CGB_RGB5(2,2,2)
};
static const UINT16 save_cgb_ui_obj_pal0[4u] = {
    CGB_RGB5(31,31,31), CGB_RGB5(23,23,20), CGB_RGB5(11,10,9), CGB_RGB5(0,0,0)
};
static UINT8 save_attr_row[20u];

static void save_cgb_init_ui_palette_once(void) BANKED {
    UINT8 i;
    UINT16 c;
    if (save_cgb_palette_ready) return;
    save_cgb_palette_ready = 1u;
    if (_cpu == CGB_TYPE) cpu_fast();
    BCPS_REG = CGB_PAL_AUTOINC;
    for (i = 0u; i < 4u; i++) {
        c = save_cgb_ui_bg_pal0[i];
        BCPD_REG = (UINT8)(c & 0xFFu);
        BCPD_REG = (UINT8)(c >> 8u);
    }
    OCPS_REG = CGB_PAL_AUTOINC;
    for (i = 0u; i < 4u; i++) {
        c = save_cgb_ui_obj_pal0[i];
        OCPD_REG = (UINT8)(c & 0xFFu);
        OCPD_REG = (UINT8)(c >> 8u);
    }
}

static void save_cgb_attr_rect0(UINT8 x, UINT8 y, UINT8 w, UINT8 h) BANKED {
    UINT8 i;
    UINT8 yy;
    if (w == 0u || h == 0u) return;
    if (w > 20u) w = 20u;
    for (i = 0u; i < w; i++) save_attr_row[i] = 0u;
    VBK_REG = VBK_ATTRIBUTES;
    for (yy = 0u; yy < h; yy++) set_bkg_tiles(x, (UINT8)(y + yy), w, 1u, save_attr_row);
    VBK_REG = VBK_TILES;
}



#define SAVE_DEBUG_BASE 0x1F00u
#define SAVE_DEBUG_MAGIC0 0x44u /* D */
#define SAVE_DEBUG_MAGIC1 0x42u /* B */
#define SAVE_DEBUG_PATTERN 0xA5u

static UINT8 *save_debug_sram_ptr(UINT16 offset) BANKED {
    return (UINT8 *)(0xA000u + SAVE_DEBUG_BASE + offset);
}

static char save_hex_digit(UINT8 v) BANKED {
    v &= 0x0Fu;
    return (char)((v < 10u) ? ('0' + v) : ('A' + (v - 10u)));
}

static void save_u8_to_hex(UINT8 value, char *out) BANKED {
    out[0] = save_hex_digit((UINT8)(value >> 4u));
    out[1] = save_hex_digit(value);
    out[2] = '\0';
}

static UINT8 save_debug_calc_checksum(UINT8 seq) BANKED {
    UINT8 sum;
    UINT8 i;
    sum = (UINT8)(SAVE_DEBUG_MAGIC0 + SAVE_DEBUG_MAGIC1 + seq);
    for (i = 0u; i < 12u; i++) {
        sum = (UINT8)(sum + (UINT8)(SAVE_DEBUG_PATTERN ^ i ^ seq));
    }
    return (UINT8)(sum ^ 0xFFu);
}

static UINT8 save_debug_pattern_ok(UINT8 seq) BANKED {
    UINT8 i;
    UINT8 ok;
    ok = 1u;
    ENABLE_RAM;
    SWITCH_RAM(0u);
    for (i = 0u; i < 12u; i++) {
        if (*save_debug_sram_ptr((UINT16)(4u + i)) != (UINT8)(SAVE_DEBUG_PATTERN ^ i ^ seq)) ok = 0u;
    }
    DISABLE_RAM;
    return ok;
}

static void save_debug_read(UINT8 *m0, UINT8 *m1, UINT8 *seq, UINT8 *chk) BANKED {
    ENABLE_RAM;
    SWITCH_RAM(0u);
    *m0 = *save_debug_sram_ptr(0u);
    *m1 = *save_debug_sram_ptr(1u);
    *seq = *save_debug_sram_ptr(2u);
    *chk = *save_debug_sram_ptr(3u);
    DISABLE_RAM;
}

static UINT8 save_debug_write_pattern(void) BANKED {
    UINT8 m0;
    UINT8 m1;
    UINT8 seq;
    UINT8 chk;
    UINT8 i;

    save_debug_read(&m0, &m1, &seq, &chk);
    if (m0 != SAVE_DEBUG_MAGIC0 || m1 != SAVE_DEBUG_MAGIC1) seq = 0u;
    seq++;
    chk = save_debug_calc_checksum(seq);

    ENABLE_RAM;
    SWITCH_RAM(0u);
    *save_debug_sram_ptr(0u) = SAVE_DEBUG_MAGIC0;
    *save_debug_sram_ptr(1u) = SAVE_DEBUG_MAGIC1;
    *save_debug_sram_ptr(2u) = seq;
    *save_debug_sram_ptr(3u) = chk;
    for (i = 0u; i < 12u; i++) {
        *save_debug_sram_ptr((UINT16)(4u + i)) = (UINT8)(SAVE_DEBUG_PATTERN ^ i ^ seq);
    }
    DISABLE_RAM;
    return seq;
}

static void save_debug_wait10(void) BANKED {
    UINT16 i;
    for (i = 0u; i < 600u; i++) {
        wait_vbl_done();
    }
}

static void save_clear_screen(void) BANKED;

static void save_debug_draw_status(UINT8 wrote, UINT8 waited) BANKED {
    UINT8 m0;
    UINT8 m1;
    UINT8 seq;
    UINT8 chk;
    UINT8 calc;
    UINT8 pat_ok;
    char hex[3];

    save_clear_screen();
    save_debug_read(&m0, &m1, &seq, &chk);
    calc = save_debug_calc_checksum(seq);
    pat_ok = save_debug_pattern_ok(seq);

    jp_put_bkg_text(1u, 1u, "SRAM DEBUG");
    jp_put_bkg_text(1u, 3u, "MAGIC");
    save_u8_to_hex(m0, hex); jp_put_bkg_text(8u, 3u, hex);
    save_u8_to_hex(m1, hex); jp_put_bkg_text(11u, 3u, hex);
    jp_put_bkg_text(1u, 4u, "SEQ");
    save_u8_to_hex(seq, hex); jp_put_bkg_text(8u, 4u, hex);
    jp_put_bkg_text(1u, 5u, "CHK");
    save_u8_to_hex(chk, hex); jp_put_bkg_text(8u, 5u, hex);
    jp_put_bkg_text(11u, 5u, (chk == calc) ? "OK" : "NG");
    jp_put_bkg_text(1u, 6u, "PAT");
    jp_put_bkg_text(8u, 6u, pat_ok ? "OK" : "NG");

    if (m0 == SAVE_DEBUG_MAGIC0 && m1 == SAVE_DEBUG_MAGIC1 && chk == calc && pat_ok) {
        jp_put_bkg_text(1u, 8u, "SRAM DATA OK");
    } else {
        jp_put_bkg_text(1u, 8u, "SRAM DATA LOST");
    }

    if (wrote) jp_put_bkg_text(1u, 10u, "WRITE DONE");
    if (waited) jp_put_bkg_text(1u, 11u, "10SEC CHECK DONE");

    jp_put_bkg_text(1u, 13u, "A WRITE TEST");
    jp_put_bkg_text(1u, 14u, "START WAIT10");
    jp_put_bkg_text(1u, 15u, "SEL REFRESH");
    jp_put_bkg_text(1u, 16u, "B BACK");
}

static void save_runtime_sram_debug_screen(void) BANKED {
    UINT8 keys;
    save_debug_draw_status(0u, 0u);
    while (1) {
        keys = audio_waitpad(J_A | J_B | J_START | J_SELECT);
        audio_waitpadup();
        if (keys & J_B) {
            return;
        } else if (keys & J_A) {
            save_debug_write_pattern();
            save_debug_draw_status(1u, 0u);
        } else if (keys & J_START) {
            jp_put_bkg_text(1u, 11u, "WAITING 10SEC...");
            save_debug_wait10();
            save_debug_draw_status(0u, 1u);
        } else if (keys & J_SELECT) {
            save_debug_draw_status(0u, 0u);
        }
    }
}


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
    save_cgb_init_ui_palette_once();
    jp_bkg_clear_area(0u, 0u, 20u, 18u);
    jp_draw_bkg_frame(0u, 0u, 20u, 18u);
    save_cgb_attr_rect0(0u, 0u, 20u, 18u);
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


static const char prologue_line0[] = "はるかな地より";
static const char prologue_line1[] = "魔王はあらわれた。";
static const char prologue_line2[] = "獣も人も姿を変えられ";
static const char prologue_line3[] = "古き都は灰となった。";
static const char prologue_line4[] = "その力の源を知る者なく";
static const char prologue_line5[] = "残された者は秘術をぬすみ";
static const char prologue_line6[] = "魔法と呼び闇にあらがう。";

static const char * const prologue_lines[] = {
    prologue_line0,
    prologue_line1,
    prologue_line2,
    prologue_line3,
    prologue_line4,
    prologue_line5,
    prologue_line6
};
#define PROLOGUE_LINE_COUNT 7u
#define PROLOGUE_LINE_SPACING_ROWS 2u

static void save_cgb_write_bg_palette4(UINT8 pal, UINT16 c0, UINT16 c1, UINT16 c2, UINT16 c3) BANKED {
    if (_cpu != CGB_TYPE) return;
    BCPS_REG = (UINT8)(CGB_PAL_AUTOINC | (UINT8)(pal << 3));
    BCPD_REG = (UINT8)(c0 & 0xFFu); BCPD_REG = (UINT8)(c0 >> 8u);
    BCPD_REG = (UINT8)(c1 & 0xFFu); BCPD_REG = (UINT8)(c1 >> 8u);
    BCPD_REG = (UINT8)(c2 & 0xFFu); BCPD_REG = (UINT8)(c2 >> 8u);
    BCPD_REG = (UINT8)(c3 & 0xFFu); BCPD_REG = (UINT8)(c3 >> 8u);
}

static void save_prologue_init_palettes(void) BANKED {
    if (_cpu == CGB_TYPE) {
        cpu_fast();
        /* White background, black text.  Palettes 1-3 are progressively
         * lighter text for the fade-in / fade-out bands.  JP glyph pixels
         * use color3, so fading is done by changing color3 per BG row.
         */
        save_cgb_write_bg_palette4(0u, CGB_RGB5(31,31,28), CGB_RGB5(23,23,21), CGB_RGB5(13,13,12), CGB_RGB5(2,2,2));
        save_cgb_write_bg_palette4(1u, CGB_RGB5(31,31,28), CGB_RGB5(28,28,26), CGB_RGB5(23,23,21), CGB_RGB5(12,12,11));
        save_cgb_write_bg_palette4(2u, CGB_RGB5(31,31,28), CGB_RGB5(30,30,28), CGB_RGB5(27,27,25), CGB_RGB5(20,20,18));
        save_cgb_write_bg_palette4(3u, CGB_RGB5(31,31,28), CGB_RGB5(31,31,28), CGB_RGB5(31,31,28), CGB_RGB5(31,31,28));
    } else {
        BGP_REG = 0xE4u;
    }
}

static UINT8 save_prologue_pixel_attr(INT16 y) BANKED {
    /* Longer invisible band at the top/bottom.  The line is already drawn
     * offscreen, so only the palette changes as it enters/exits the view.
     */
    if (y < 32 || y > 111) return 3u;
    if (y < 48 || y > 95) return 2u;
    if (y < 64 || y > 79) return 1u;
    return 0u;
}

static void save_prologue_wait(UINT8 frames) BANKED {
    while (frames--) audio_wait_vbl();
}

static void save_prologue_fill_attr_row(UINT8 attr) BANKED {
    save_attr_row[0] = attr; save_attr_row[1] = attr; save_attr_row[2] = attr; save_attr_row[3] = attr;
    save_attr_row[4] = attr; save_attr_row[5] = attr; save_attr_row[6] = attr; save_attr_row[7] = attr;
    save_attr_row[8] = attr; save_attr_row[9] = attr; save_attr_row[10] = attr; save_attr_row[11] = attr;
    save_attr_row[12] = attr; save_attr_row[13] = attr; save_attr_row[14] = attr; save_attr_row[15] = attr;
    save_attr_row[16] = attr; save_attr_row[17] = attr; save_attr_row[18] = attr; save_attr_row[19] = attr;
}

static void save_prologue_set_attr_row(UINT8 y, UINT8 attr) BANKED {
    if (_cpu != CGB_TYPE) return;
    save_prologue_fill_attr_row(attr);
    VBK_REG = VBK_ATTRIBUTES;
    set_bkg_tiles(0u, y, 20u, 1u, save_attr_row);
    VBK_REG = VBK_TILES;
}

static void save_prologue_clear_row(UINT8 y) BANKED {
    jp_bkg_clear_area(0u, y, 20u, 1u);
    if (_cpu == CGB_TYPE) save_prologue_set_attr_row(y, 0u);
}

#define PROLOGUE_START_ROW 18u
#define PROLOGUE_START_PX  144u

static UINT8 save_prologue_text_cols(const char *s) BANKED {
    UINT8 i;
    UINT8 cols;
    UINT8 c;

    i = 0u;
    cols = 0u;
    while (s[i] != '\0') {
        c = (UINT8)s[i];
        if ((c & 0x80u) == 0u) {
            i++;
        } else if ((c & 0xE0u) == 0xC0u) {
            i = (UINT8)(i + 2u);
        } else if ((c & 0xF0u) == 0xE0u) {
            i = (UINT8)(i + 3u);
        } else {
            i++;
        }
        cols++;
    }
    return cols;
}

static UINT8 save_prologue_center_x(const char *s) BANKED {
    UINT8 cols;
    cols = save_prologue_text_cols(s);
    if (cols >= 20u) return 0u;
    return (UINT8)((20u - cols) >> 1u);
}

static void save_prologue_draw_all_offscreen(void) BANKED {
    UINT8 i;
    UINT8 y;

    jp_bkg_clear_area(0u, 0u, 20u, 32u);
    if (_cpu == CGB_TYPE) save_cgb_attr_rect0(0u, 0u, 20u, 32u);

    /* Draw the whole prologue once into BG rows that are outside the initial
     * 20x18 visible area.  From here on, motion is SCY-only, so there is no
     * left-to-right glyph generation or row jitter while the text is visible.
     */
    for (i = 0u; i < PROLOGUE_LINE_COUNT; i++) {
        y = (UINT8)(PROLOGUE_START_ROW + (UINT8)(i * PROLOGUE_LINE_SPACING_ROWS));
        jp_put_bkg_text(save_prologue_center_x(prologue_lines[i]), y, prologue_lines[i]);
        save_prologue_set_attr_row(y, 3u);
    }
}

static void save_prologue_update_fade(UINT16 scroll) BANKED {
    UINT8 i;
    UINT8 y;
    UINT8 attr;
    INT16 y_px;

    if (_cpu != CGB_TYPE) return;

    for (i = 0u; i < PROLOGUE_LINE_COUNT; i++) {
        y = (UINT8)(PROLOGUE_START_ROW + (UINT8)(i * PROLOGUE_LINE_SPACING_ROWS));
        y_px = (INT16)((INT16)PROLOGUE_START_PX + (INT16)((UINT16)i * 16u) - (INT16)scroll);
        attr = save_prologue_pixel_attr(y_px);
        save_prologue_set_attr_row(y, attr);
    }
}

static void save_prologue_draw_skip_cursor(UINT8 cursor) BANKED {
    jp_put_bkg_text(6u, 11u, (cursor == 0u) ? "▶はい" : " はい");
    jp_put_bkg_text(12u, 11u, (cursor == 1u) ? "▶いいえ" : " いいえ");
}

static UINT8 save_prologue_confirm_skip(void) BANKED {
    UINT8 cursor;
    UINT8 keys;
    UINT8 prev;
    UINT8 new_keys;

    cursor = 1u;
    move_bkg(0u, 0u);
    jp_bkg_clear_area(0u, 0u, 20u, 18u);
    if (_cpu == CGB_TYPE) save_cgb_attr_rect0(0u, 0u, 20u, 18u);
    prev = joypad();
    jp_draw_bkg_frame(2u, 6u, 16u, 8u);
    jp_put_bkg_text(4u, 8u, "スキップしますか？");
    save_prologue_draw_skip_cursor(cursor);

    while (1) {
        keys = joypad();
        new_keys = (UINT8)(keys & (UINT8)(~prev));
        if (new_keys & (J_LEFT | J_RIGHT | J_UP | J_DOWN)) {
            cursor ^= 1u;
            save_prologue_draw_skip_cursor(cursor);
        } else if (new_keys & J_A) {
            return (UINT8)(cursor == 0u);
        } else if (new_keys & (J_B | J_START)) {
            return 0u;
        }
        prev = keys;
        audio_wait_vbl();
    }
}

static void save_runtime_play_prologue(void) BANKED {
    UINT16 scroll;
    UINT16 max_scroll;
    UINT8 keys;
    UINT8 prev;
    UINT8 new_keys;
    UINT8 delay;
    UINT8 step;

    DISPLAY_OFF;
    HIDE_WIN;
    SHOW_BKG;
    HIDE_SPRITES;
    move_bkg(0u, 0u);
    save_prologue_init_palettes();
    save_prologue_draw_all_offscreen();
    DISPLAY_ON;

    prev = joypad();
    scroll = 0u;
    max_scroll = 248u;

    while (scroll < max_scroll) {
        move_bkg(0u, (UINT8)scroll);
        save_prologue_update_fade(scroll);

        keys = joypad();
        new_keys = (UINT8)(keys & (UINT8)(~prev));
        if (new_keys & J_START) {
            if (save_prologue_confirm_skip()) break;
            /* The confirmation window clears the screen, so rebuild the
             * prologue view at the current scroll position after returning.
             */
            save_prologue_init_palettes();
            save_prologue_draw_all_offscreen();
        }
        prev = keys;

        if (keys & J_A) {
            step = 3u;
            delay = 1u;
        } else {
            step = 1u;
            delay = 16u;
        }
        save_prologue_wait(delay);
        scroll = (UINT16)(scroll + step);
    }

    move_bkg(0u, 0u);
    save_clear_screen();
    if (_cpu == CGB_TYPE) {
        save_cgb_attr_rect0(0u, 0u, 20u, 18u);
        save_cgb_palette_ready = 0u;
        save_cgb_init_ui_palette_once();
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
        keys = audio_waitpad(J_UP | J_DOWN | J_A | J_START | J_SELECT);
        audio_waitpadup();
        if (keys & (J_UP | J_DOWN)) {
            save_cursor ^= 1u;
            save_draw_cursor(8u, 2u);
        } else if (keys & J_SELECT) {
            save_runtime_sram_debug_screen();
            save_clear_screen();
            jp_put_bkg_text(5u, 3u, "GBDK RPG");
            jp_put_bkg_text(4u, 8u, "はじめから");
            jp_put_bkg_text(4u, 10u, "つづきから");
            jp_put_bkg_text(1u, 16u, "A けってい");
            save_draw_cursor(8u, 2u);
        } else if (keys & (J_A | J_START)) {
            if (save_cursor == 0u) {
                save_runtime_play_prologue();
                return SAVE_TITLE_NEW;
            }
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
