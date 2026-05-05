#include <gb/gb.h>
#include <string.h>
#include "jpfont.h"
typedef struct JpCacheEntry {
    UINT16 code;
    UINT8 tile;
    UINT16 stamp;
    UINT8 used;
} JpCacheEntry;

static JpCacheEntry jp_cache[JP_CACHE_SIZE];
static UINT16 jp_cache_clock = 1u;
static UINT8 jp_next_tile = JP_TILE_BASE;

/* 黒枠・白背景 */
static const unsigned char jp_frame_tiles[] = {
    /* 0: interior white */
    0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,
    /* 1: horizontal border */
    0xFF,0xFF, 0xFF,0xFF, 0xFF,0x00, 0xFF,0x00, 0xFF,0x00, 0xFF,0x00, 0xFF,0xFF, 0xFF,0xFF,
    /* 2: vertical border */
    0xFF,0xFF, 0xFF,0x81, 0xFF,0x81, 0xFF,0x81, 0xFF,0x81, 0xFF,0x81, 0xFF,0x81, 0xFF,0xFF,
    /* 3: top-left */
    0xFF,0xFF, 0xFF,0xFF, 0xFF,0x80, 0xFF,0x80, 0xFF,0x80, 0xFF,0x80, 0xFF,0x80, 0xFF,0x80,
    /* 4: top-right */
    0xFF,0xFF, 0xFF,0xFF, 0xFF,0x01, 0xFF,0x01, 0xFF,0x01, 0xFF,0x01, 0xFF,0x01, 0xFF,0x01,
    /* 5: bottom-left */
    0xFF,0x80, 0xFF,0x80, 0xFF,0x80, 0xFF,0x80, 0xFF,0x80, 0xFF,0x80, 0xFF,0xFF, 0xFF,0xFF,
    /* 6: bottom-right */
    0xFF,0x01, 0xFF,0x01, 0xFF,0x01, 0xFF,0x01, 0xFF,0x01, 0xFF,0x01, 0xFF,0xFF, 0xFF,0xFF,
    /* 7: blank white */
    0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00
};

/* 最終版: 美咲フォントを90度右回転して表示 */
static void jp_rotate_right(const byte *src, byte *dst) {
    UINT8 y;
    UINT8 x;
    byte row;

    for (y = 0u; y < 8u; y++) dst[y] = 0u;

    for (y = 0u; y < 8u; y++) {
        row = src[y];
        for (x = 0u; x < 8u; x++) {
            if (row & (1u << x)) {
                dst[x] |= (byte)(1u << (7u - y));
            }
        }
    }
}

/* 白背景に濃い文字。
 * v3: 会話Windowの文字背景を完全な白(color0)にする。
 *     glyph pixels are color3 by setting both bitplanes only where the glyph exists.
 */
static void jp_font_to_tile(const byte *src, unsigned char *dst) {
    byte rotated[8];
    UINT8 y;

    jp_rotate_right(src, rotated);

    for (y = 0u; y < 8u; y++) {
        dst[y * 2u] = rotated[y];
        dst[y * 2u + 1u] = rotated[y];
    }
}

void jp_init(void) {
    set_bkg_data(JP_FRAME_BASE, 8u, jp_frame_tiles);
    jp_reset_cache();
    jp_window_hide();
}

void jp_reset_cache(void) {
    UINT8 i;
    jp_next_tile = JP_TILE_BASE;
    jp_cache_clock = 1u;

    for (i = 0u; i < JP_CACHE_SIZE; i++) {
        jp_cache[i].used = 0u;
        jp_cache[i].code = 0u;
        jp_cache[i].tile = 0u;
        jp_cache[i].stamp = 0u;
    }
}

static UINT8 jp_cache_find(UINT16 code) {
    UINT8 i;
    for (i = 0u; i < JP_CACHE_SIZE; i++) {
        if (jp_cache[i].used && jp_cache[i].code == code) {
            jp_cache[i].stamp = jp_cache_clock++;
            return jp_cache[i].tile;
        }
    }
    return 0u;
}

static UINT8 jp_cache_store(UINT16 code, const byte *font8) {
    UINT8 i;
    UINT8 slot = 0u;
    UINT16 oldest = 0xFFFFu;
    unsigned char tile[16];

    jp_font_to_tile(font8, tile);

    for (i = 0u; i < JP_CACHE_SIZE; i++) {
        if (!jp_cache[i].used) {
            slot = i;
            break;
        }
        if (jp_cache[i].stamp < oldest) {
            oldest = jp_cache[i].stamp;
            slot = i;
        }
    }

    if (!jp_cache[slot].used) {
        jp_cache[slot].tile = jp_next_tile++;
        if (jp_next_tile >= (UINT8)(JP_TILE_BASE + JP_CACHE_SIZE)) {
            jp_next_tile = JP_TILE_BASE;
        }
    }

    set_bkg_data(jp_cache[slot].tile, 1u, tile);
    jp_cache[slot].used = 1u;
    jp_cache[slot].code = code;
    jp_cache[slot].stamp = jp_cache_clock++;
    return jp_cache[slot].tile;
}

static UINT8 jp_get_tile_for_utf8(const char *p, UINT8 *consumed) {
    UINT8 cached;
    UINT16 cache_key;
    UINT16 utf16;
    byte font8[8];
    const char *next;
    byte consumed_len;

    if (p == NULL || *p == '\0') {
        *consumed = 0u;
        return JP_FRAME_BASE + 0u;
    }

    /*
     * rpg069:
     * Restore the original normalized UTF-16 cache key now that misakiUTF16.c
     * code is non-banked again.  rpg064-rpg068 used a byte-based fallback key
     * to avoid nested BANKED calls, but that can collide and reuse the wrong
     * glyph tile, causing battle/dialogue text corruption.
     */
    consumed_len = charUFT8toUTF16(&utf16, p);
    if (consumed_len == 0u) {
        *consumed = 1u;
        return JP_FRAME_BASE + 0u;
    }

    utf16 = utf16_HantoZen(utf16);
    cache_key = utf16;

    cached = jp_cache_find(cache_key);
    if (cached != 0u) {
        *consumed = consumed_len;
        return cached;
    }

    next = getFontData(font8, p, 1);
    if (next == NULL) {
        *consumed = consumed_len;
        return JP_FRAME_BASE + 0u;
    }

    *consumed = (UINT8)(next - p);
    return jp_cache_store(cache_key, font8);
}

void jp_window_hide(void) {
    HIDE_WIN;
}

void jp_window_prepare(void) {
    UINT8 x, y;
    unsigned char t;

    for (y = 0u; y < JP_WIN_H; y++) {
        for (x = 0u; x < JP_WIN_W; x++) {
            t = JP_FRAME_BASE + 0u;
            if (y == 0u && x == 0u) t = JP_FRAME_BASE + 3u;
            else if (y == 0u && x == (UINT8)(JP_WIN_W - 1u)) t = JP_FRAME_BASE + 4u;
            else if (y == (UINT8)(JP_WIN_H - 1u) && x == 0u) t = JP_FRAME_BASE + 5u;
            else if (y == (UINT8)(JP_WIN_H - 1u) && x == (UINT8)(JP_WIN_W - 1u)) t = JP_FRAME_BASE + 6u;
            else if (y == 0u || y == (UINT8)(JP_WIN_H - 1u)) t = JP_FRAME_BASE + 1u;
            else if (x == 0u || x == (UINT8)(JP_WIN_W - 1u)) t = JP_FRAME_BASE + 2u;
            set_win_tiles(x, y, 1u, 1u, &t);
        }
    }
    move_win(JP_WIN_X, JP_WIN_Y);
    SHOW_WIN;
}

void jp_window_clear_text(void) {
    UINT8 x, y;
    unsigned char t = JP_FRAME_BASE + 0u;
    for (y = 1u; y <= JP_TEXT_ROWS; y++) {
        for (x = 1u; x <= JP_TEXT_COLS; x++) {
            set_win_tiles(x, y, 1u, 1u, &t);
        }
    }
}

UINT8 jp_put_glyph_utf8(UINT8 col, UINT8 row, const char *p, UINT8 *consumed) {
    UINT8 tile = jp_get_tile_for_utf8(p, consumed);
    set_win_tiles(col, row, 1u, 1u, &tile);
    return tile;
}

void jp_bkg_clear_area(UINT8 x0, UINT8 y0, UINT8 w, UINT8 h) {
    UINT8 x;
    UINT8 y;
    unsigned char blank = JP_FRAME_BASE + 0u;
    for (y = 0u; y < h; y++) {
        for (x = 0u; x < w; x++) {
            set_bkg_tiles((UINT8)(x0 + x), (UINT8)(y0 + y), 1u, 1u, &blank);
        }
    }
}

void jp_put_bkg_text(UINT8 col, UINT8 row, const char *text) {
    UINT8 consumed;
    UINT8 tile;
    const char *p = text;
    UINT8 x = col;

    while (*p) {
        if (*p == '\n') {
            row++;
            x = col;
            p++;
            continue;
        }
        tile = jp_get_tile_for_utf8(p, &consumed);
        set_bkg_tiles(x, row, 1u, 1u, &tile);
        p += (consumed == 0u) ? 1 : consumed;
        x++;
        if (x >= 20u) break;
    }
}

void jp_wait_vbl(UINT8 n) {
    while (n--) wait_vbl_done();
}
