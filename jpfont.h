#ifndef JPFONT_H
#define JPFONT_H

#include <gb/gb.h>
#include "misakiUTF16.h"

#define JP_CACHE_SIZE   48u
#define JP_TILE_BASE    128u
#define JP_FRAME_BASE   176u

#define JP_WIN_X        7u
#define JP_WIN_Y        112u
#define JP_WIN_W        20u
#define JP_WIN_H        4u

#define JP_TEXT_COLS    18u
#define JP_TEXT_ROWS    2u

void jp_init(void);
void jp_reset_cache(void);
void jp_window_hide(void);
void jp_window_prepare(void);
void jp_window_clear_text(void);
UINT8 jp_put_glyph_utf8(UINT8 col, UINT8 row, const char *p, UINT8 *consumed);
void jp_bkg_clear_area(UINT8 x0, UINT8 y0, UINT8 w, UINT8 h);
void jp_put_bkg_text(UINT8 col, UINT8 row, const char *text);
void jp_wait_vbl(UINT8 n);

#endif
